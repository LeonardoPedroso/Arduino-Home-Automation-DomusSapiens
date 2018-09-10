/* Ficheiro deviceCommunication.cpp
 * Neste ficheiro estão definidas as funções necessárias à comunicação com os dispositivos secundários
 * Está neste ficheiro também definido o protocolo usado para o pairing (handshake) com os dispositivos
 * Ente handskake consiste em: 1) procurar uma mensagem de tentativa de emparelhamento de um dispositivo
 *                             2) gerar uma chave privada, gerar a pública e enviá-la
 *                             3) receber a chave publica do dispositivo e calcular o shared secret com base na privada do central
 *                             4) guardar o shared secret
 * Este é algoritmo de Diffie Hellman
 * 
 */

//------------------------------------------- INCLUDES ------------------------------------
#include "deviceCommunication.h"
#include "StorageManagement.h"
#include "deviceCommunication.h"

using namespace StorageManagement;

//-------------------------------------- RF BUFFER DATA ----------------------------------
uint8_t rfBuffer[BUFFER_LENGTH+1];
uint8_t rfBufferLen = BUFFER_LENGTH +1 ;


//--------------------------------------- FUNÇÕES --------------------------------------------

//Função que prepara a comunicação com os dispositivos secundários
//Inicia a comunicação rf através da biblioteca VirtualWire
void deviceCommunication::initDeviceCommunication()
{
  //define os pins de TX e RX
  vw_set_tx_pin(DEVICE_COMMUNICATION_TX_PIN); 
  vw_set_rx_pin(DEVICE_COMMUNICATION_RX_PIN);
  //vw_set_ptt_pin(5);
  vw_set_ptt_inverted(true);  //Necessário para o módulo que usei
  //Definir a velocidade de transferencia de dados
  vw_setup(DEVICE_COMMUNICATION_BAUD_RATE);
  #ifdef DEBUG 
  Serial.println(F("Main ready"));
  #endif
}

//Função que envia toda a informação guardada em EEPROM para os respetivos dispositivos
//Se a energia falhar os dispositivos são atualizados automaticamente, ainda que seja possível
//atualizá-los manualmente através da app
void deviceCommunication::setUpDevices()
{
  DeviceData device((char*)NULL);
  //itera os blocos com informação guardada na EEPROM
  for (byte i = 0; ; i++) {
    if (!device.getDeviceFromEeprom(i)) 
      break;
    device.sendUpdatedInfo(); //envia o update
    device.sendTime(); //atualiza a hora
  }

}


//Função que envia um array de 16 bytes e aguarda pela confirmação de resposta do dispositivo
//Retorna TRUE se a mensagem foi enviada e recebida pelo dispositivo com sucesso e FALSE em caso contrário
byte deviceCommunication::sendByteArray(DeviceData * deviceAp, byte * byteArray, byte byteArrayLen)
{
  byte sentMsgs = 0; //contagem do número de x que a mensagem foi enviada
  do {
    if (sentMsgs == 10)
      break;
    vw_send(byteArray, DEFAULT_BYTE_ARRAY_LENGTH); //envia  a mensagem 
    sentMsgs++;
    vw_rx_start();
  } while (!(deviceCommunication::checkReception(deviceAp->id))); //espera algum tempo pela confirmação de recebimento
  //se não tiver recebido a aconfoirmação tenta outra vez até um máximo de 5x
  vw_rx_stop();
  if (sentMsgs < 10)
    return TRUE;
  return FALSE;
}

//Função que aguarda por uma mensagem de confirmação de receção proveninte do dispositivo secundário para 
//onde foi enviada a mensagem
//Retorna TRUE se intercetar uma mensagem de confirmação de receção e FALSE caso contrário
byte deviceCommunication::checkReception(unsigned int id)
{ 
  //Espera um tempo máximo de RESPONSE_WAIT_TIMEOUT para receber a mensagem de confirmação de receção
  if (vw_wait_rx_max(RESPONSE_WAIT_TIMEOUT) && vw_get_message(rfBuffer, &rfBufferLen))
  {
    if (rfBuffer[0] == CONFIRMATION_MESSAGE) { //se a mensagem for do tipo de confirmação de receção
      unsigned int receivedId = 0;
      for (byte i = 1; i <= 5; i++) //identifica o dispositivo que emitiu a mensagem
        receivedId += StorageManagement::power(10, 5 - i) * (rfBuffer[i] - '0'); 
      if (receivedId == id) //se for o dispostivo que estavamos à espera que confirmasse a receção
        return TRUE;
    }
  }
  return FALSE;
}

//Função responsável pelo hanshake, ie pairing, com os dispositivo secundário
//Retorna TRUE se foi bem sucedido e FALSE caso contrário
byte deviceCommunication::handshakeProtocol(DeviceData * device)
{
  #ifdef DEBUG
  Serial.println(F("Starting Handskake - waiting for message"));
  #endif

  for (byte i = 0; i < 16; i++) //faz reset do shared secret do dispositivo
    *(device->sharedSecret + i) = 0;

  vw_rx_start();
  //espera por uma mensagem de um dispositivo de pedido de emparelhamento
  if (vw_wait_rx_max(30000) && vw_get_message(rfBuffer, &rfBufferLen)) {  //DEFINE AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
    Serial.println("REcebi");
    Serial.flush();
    Serial.println(rfBuffer[0]);
    if (rfBuffer[0] == PAIRING) {
      unsigned int receivedId = 0;
      for (byte i = 1; i <= 5; i++) //identifica o ID do emissor
        receivedId += StorageManagement::power(10, 5 - i) * (rfBuffer[i] - '0');
      device->id = receivedId;

      //cria a chave privada do dispositivo central aleatoreamente 
      uint64_t mainPrivate1 = randomint64();
      uint64_t mainPrivate2 = randomint64();
      //calcula a chave publica do dispositivo central através da chave privada determinada anteriormente
      uint64_t mainPublic1 = compute(G, mainPrivate1, P);
      uint64_t mainPublic2 = compute(G, mainPrivate2, P);
      byte codeM[17];
      codeM[0] = CODE; //cria o vetor de bytes a ser enviado
      
      //conversão em array de bytes da chave pública
      byte * p = (byte *) &mainPublic1;
      for (byte i = 0; i < 8; i++)
        codeM[8 - i] = *(p + i);
      p = (byte *) &mainPublic2;
      for (byte i = 0; i < 8; i++)
        codeM[16 - i] = *(p + i);

      #ifdef DEBUG
      Serial.println(F("Sent: "));
      for (byte i = 1; i <= 16; i++)
      {
        Serial.print(codeM[i], HEX);
        Serial.print(F(" "));
      } Serial.println(F(" "));
      Serial.flush();
      #endif

      //Envia o código até receber o código público do dispositivo secundário
      byte count = 0;
      do {
        count++;
        vw_send(codeM, DEFAULT_BYTE_ARRAY_LENGTH+1); //envia o código
        vw_wait_tx();
        #ifdef DEBUG
        Serial.println(F("Sent code"));
        #endif
        //Envia o código no máximo 15x vezes
        if (count == 30) //---------------------------------------------  DEFINE AQUIIIIIIIIIIIIIIIII
          return FALSE;
      }while (!deviceCommunication::getCode(codeM));

      #ifdef DEBUG
      Serial.println("Received:");
      for (byte i = 0; i < 16; i++)
      {
        Serial.print(codeM[i], HEX);
        Serial.print(F(" "));
      } Serial.println(F(" "));
      Serial.flush();
      #endif

      //converte chave recebida sob a forma de array de bytes em dois uint64_t
      mainPublic1 = 0;
      for (byte i = 0; i < 8; i++)
        mainPublic1 += (uint64_t)codeM[i] << 8 * (7 - i);
      mainPublic2 = 0;
      for (byte i = 0; i < 8; i++)
        mainPublic2 += (uint64_t)codeM[i + 8] << 8 * (7 - i);

      //Calcula o shared secret a partir da chave publica do dispositivo e da chave privada do dispositovo central
      mainPublic1 =  compute(mainPublic1, mainPrivate1, P);
      mainPublic2 =  compute(mainPublic2, mainPrivate2, P);

      //verifica que a chave foi de facto recebida corretamente  e que o shared secret não é nulo
      count = FALSE;
      for (byte i = 0; i = 8; i++) {
        if (mainPublic1 != 0) {
          count = TRUE;
          break;
        }
      }
      if (!count)
        return FALSE;
      count = FALSE;
      for (byte i = 0; i = 8; i++) {
        if (mainPublic2 != 0) {
          count = TRUE;
          break;
        }
      }
      if (!count)
        return FALSE;

      //converte os 2 uint_64t num array de bytes que são gusdados no objeto do dispositivo e posteriormenet em EEPROM
      p = (byte *) &mainPublic1;
      for (byte i = 0; i < 8; i++)
        *(device->sharedSecret + 7 - i) = *(p + i);
      p = (byte *) &mainPublic2;
      for (byte i = 0; i < 8; i++)
        *(device->sharedSecret + 15 - i) = *(p + i);

     /* #ifdef DEBUG
      Serial.println(F("Shared secret: "));
      for (byte i = 0; i < 16; i++) {
        Serial.print(*(device->sharedSecret + i), HEX);
        Serial.print(F(" "));
      }
      Serial.println(F(" "));
      Serial.flush();
      #endif*/
      
      vw_rx_stop();
      return TRUE;
    }

    #ifdef DEBUG
    Serial.println(F("Na ta ca nada"));
    #endif
    
    vw_rx_stop();
    return FALSE;
  }

  return FALSE;  
}

//Função que aguarda a receção de uma mensagem que contenha um código que guarda no vetor que é passado por endereço
//Retorna TRUE se recebeu um código válido e FALSE caso contrário
byte deviceCommunication::getCode(byte * code128bit)
{
  vw_rx_start();
  #ifdef DEBUG
  Serial.println(F("Trying to get code"));
  #endif

  //Espera o recebimento de uma mensagem 
  if (vw_wait_rx_max(RESPONSE_WAIT_TIMEOUT) && vw_get_message(rfBuffer, &rfBufferLen))
  {
    vw_rx_stop();
    if (rfBuffer[0] == CODE) { //verifica se é um código
      #ifdef DEBUG
      Serial.println(F("Código recebido"));
      #endif
      for (byte i = 1; i <= 16; i++) {
        *(code128bit + i - 1) = rfBuffer[i]; //extrai o códido do buffer da comunicação rf
        //Serial.println(*(code128bit + i - 1), HE);
      }
      return TRUE;
    }
  }
  vw_rx_stop();
  return FALSE;
}


