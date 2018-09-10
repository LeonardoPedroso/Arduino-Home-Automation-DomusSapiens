/*
 * Ficheiro DeviceData.cpp
 * Este ficheiro é o que contem as funções que regulam a comunicação com os vários dispositivos 
 * secundários e gerem a informação guardada, para isso usa as funções dos ficheiros "StorageManagement.h"
 * e "deviceCommunication.h".
 * Define também a classe DeviceData que permite fazer esta gestão de forma mais organizada e eficiente
 */

//------------------------------------ INCLUDES ----------------------------------------
#include "DeviceData.h"
#include "StorageManagement.h"
#include "deviceCommunication.h"

using namespace StorageManagement;
using namespace deviceCommunication;

//instância da classe AES que permite encryptar a comunicação com os dispositivos secundários
AES aes; 

//------------------------------------ FUNÇÕES --------------------------------------

//---------------------------- CONSTRUCTORS ---------------------------------
//Constructor que instancia um objeto a partir de uma string em formato JSON
DeviceData::DeviceData(char * json)
{
  if (json){
    
    //extrai o id
    unsigned int id = 0;
    for (byte i = 1; i <= 5; i++)
      id += ((*(json + i) - '0')) * StorageManagement::power(10, 5 - i);
    this->id = id;
    json += 6;

    //extrai o nome
    byte i;
    for (i = 0; ; i++) {
      if (*(json + i) <= '9' && *(json + i) != SPACE_MARKER) {
        this->nameDev[i] = '\0';
        break;
      } else
        this->nameDev[i] = *(json + i);
    }

    for (byte j = i + 1; j < 15; j++)
      this->nameDev[j] = '\0';
    json += i;
    
    //extrai o alarm up
    this->hourAlarmUp = (*(json) - '0') * 10 + (*(json + 1) - '0');
    this->minuteAlarmUp = (*(json + 2) - '0') * 10 + (*(json + 3) - '0');
    this->freqUp = *(json + 4);

    //extrai o alarm dw
    this->hourAlarmDw = 10 * (*(json + 5) - '0') + (*(json + 6) - '0');
    this->minuteAlarmDw = 10 * (*(json + 7) - '0') + (*(json + 8) - '0');
    this->freqDw = *(json + 9);
    json += 10;
    
  }

}

//Constructor alternativo
DeviceData::DeviceData(char * json, const byte mode)
{
  switch (mode) {
    case ID_ONLY: {
        unsigned int id = 0;
        for (byte i = 1; i <= 5; i++)
          id += ((long)(*(json + i) - '0')) * StorageManagement::power(10, 5 - i);
        this->id = id;
        break;
      }
  }
}

//Constructor que instancia um objeto apenas com um id fornecido com argumento
DeviceData::DeviceData(unsigned int id)
{
  this->id = id;
}


//-------------------------GESTÃO DE DADOS GUARDADOS ------------------------
//Função que faz upload 
byte DeviceData::getDeviceFromEeprom(byte deviceN)
{
  return StorageManagement::readFromEEPROM(this, deviceN);
}

//Função que guarda (ou atualiza) a informação de um dispositivo em EEPROM
byte DeviceData::saveOnEeprom()
{
#ifndef NO_DEBUG
  Serial.println(F("Saving"));
  Serial.flush();
#endif
  return StorageManagement::saveOnEEPROM(this);
}

//Função que elimina informação de um dispositivo em EEPROM
byte DeviceData::removeFromEeprom()
{
#ifndef NO_DEBUG
  Serial.println(F("Removing from EEPROM"));
  Serial.flush();
#endif
  return StorageManagement::deleteFromEEPROM(this);
}

//Função que faz apenas o upload do sharedSecret da EEPROM
byte DeviceData::getSharedSecretFromEeprom()
{
  return StorageManagement::getSharedSecretFromEeprom(this);
}


//-------------------------JSON STRINGS ------------------------

//Função que recebe um apontador para uma string na qual vai escrever toda a informação do dispositivo 
//no formato JSON
void DeviceData::getJsonString(char * json)
{ 
  //Nome
  *json = '{';
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = NAME;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  *(json + 5) = STRING_DELIMITATOR;
  json += 6; 
  byte i;
  for (i = 0; ; i++) {
    if (*(this->nameDev + i) == '\0')
      break;
    else
      *(json + i) = *(this->nameDev + i);
  }
  *(json + i) = STRING_DELIMITATOR;
  *(json + i + 1) = COMA;
  json += i + 1;


  //ID
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = ID;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  *(json + 5) = STRING_DELIMITATOR;
  sprintf(json + 6, "%05u", this->id);
  *(json + 11) = STRING_DELIMITATOR;
  *(json + 12) = COMA;
  json += 12;

  //Alarm UP
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = HOUR_ALARM_UP;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  sprintf(json + 5, "%d", this->hourAlarmUp);
  if (this->hourAlarmUp < 10)
    json += 6;
  else
    json += 7;
  *json = COMA;

  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = MINUTE_ALARM_UP;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  sprintf(json + 5, "%d", this->minuteAlarmUp);
  if (this->minuteAlarmUp < 10)
    json += 6;
  else
    json += 7;
  *json = COMA;

  //Alarm Down
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = HOUR_ALARM_DOWN;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  sprintf(json + 5, "%d", this->hourAlarmDw);
  if (this->hourAlarmDw < 10)
    json += 6;
  else
    json += 7;
  *json = COMA;

  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = MINUTE_ALARM_DOWN;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  sprintf(json + 5, "%d", this->minuteAlarmDw);
  if (this->minuteAlarmDw < 10)
    json += 6;
  else
    json += 7;
  *json = COMA;
  
  //Alarm Frequency UP
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = FREQ_ALARM_UP;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  *(json + 5) = STRING_DELIMITATOR;
  *(json + 6) = this->freqUp;
  *(json + 7) = STRING_DELIMITATOR;
  json += 8;
  *json = COMA;

  //Alarm Frequency Down
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = FREQ_ALARM_DOWN;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  *(json + 5) = STRING_DELIMITATOR;
  *(json + 6) = this->freqDw;
  *(json + 7) = STRING_DELIMITATOR;
  *(json + 8) = '}';
  *(json + 9) = '\0';

}

//Função que cria a string em formato JSON por forma a transmitir para a aplicação o
//ID do dispositivo que acabou de ser emparelhado
void DeviceData::getJsonStringParing(char * json)
{
  *json = '{';
  *(json + 1) = STRING_DELIMITATOR;
  *(json + 2) = PARING_DEVICE;
  *(json + 3) = STRING_DELIMITATOR;
  *(json + 4) = TWO_DOTS;
  *(json + 5) = STRING_DELIMITATOR;
  sprintf(json + 6, "%05u", this->id);
  *(json + 11) = STRING_DELIMITATOR;
  *(json + 12) = '}';
  *(json + 13) = '\0';
}


//------------------------- PAIRING ------------------------

//Função que procura um dispositivo com que fazer pairing e o guarda em memória
//Retorn TRUE se o pairing é bem sucedido e FALSE caso contário
byte DeviceData::getParingDevice() {

  #ifdef DEBUG
  Serial.println(F("PAIRING"));
  #endif
  
  if (deviceCommunication::handshakeProtocol(this)) {
    //remove da memória um disposivo com o mesmo id para garantir a inexistencia de definições duplas
    this->removeFromEeprom(); 
    this->hourAlarmUp = 61; //indicador de falta de informação dada pelo utilizador
    this->saveOnEeprom();
    return TRUE;
  }
  return FALSE;
}


//------------------------- COMMANDS ------------------------
//Função que envia o comando de abertura para o dispositivo secundário
//Este comando está encriptado 
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::openCmd()
{
  #ifdef DEBUG
  Serial.println(F("Open"));
  Serial.flush();
  #endif

  //Criação da string que será enviada
  char openCmd[16];
  sprintf(openCmd, "%05u", this->id); //id
  openCmd[5] = OPEN; //identificação do comando

  //geração aleatória dos restantes bytes por forma a prefazer os 16 bytes,
  //o comprimeto standard da mensagem na comunicação com os dispositivos secundários
  byte i = 0;
  for (byte k = 6; k < 15; k++) {
    openCmd[k] = rand() % 256;
    i += openCmd[k];
  }
  openCmd[15] = i; //checkSum no final

  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)openCmd, (byte *)openCmd); //encripta a mensagem com o sharedSecret

  //Envia o comando e aguarda a confirmação de receção
  if (deviceCommunication::sendByteArray(this, (byte *)openCmd, DEFAULT_BYTE_ARRAY_LENGTH))
    return TRUE;

  return FALSE;
}


//Função que envia o comando de fecho para o dispositivo secundário,
//é muito semelhante à função de abertura, mas por forma a melhorar a leitura
//do código optei por separar as funções
//Este comando está encriptado 
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::closeCmd()
{
  #ifdef DEBUG
  Serial.println(F("Close"));
  Serial.flush();
  #endif

  //Criação da string que será enviada
  char closeCmd[16]; 
  sprintf(closeCmd, "%05u", this->id); //id
  closeCmd[5] = CLOSE; //identificação do comando

  //geração aleatória dos restantes bytes por forma a prefazer os 16 bytes,
  //o comprimeto standard da mensagem na comunicação com os dispositivos secundários
  byte i = 0;
  for (byte k = 6; k < 15; k++) {
    closeCmd[k] = rand() % 256;
    i += closeCmd[k];
  }
  closeCmd[15] = i;

  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)closeCmd, (byte *)closeCmd); //encripta a mensagem com o sharedSecret

  //Envia o comando e aguarda a confirmação de receção
  if (deviceCommunication::sendByteArray(this, (byte *) closeCmd, DEFAULT_BYTE_ARRAY_LENGTH))
    return TRUE;

  return FALSE;
}

//Função que envia uma atualização de informação para um dispositivo secundário
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::sendUpdatedInfo()
{
  #ifndef NO_DEBUG
  Serial.println(F("Update device"));
  Serial.flush();
  #endif

  char updateCmd[16];
  sprintf(updateCmd, "%05u", this->id); //id
  //Informação ordenada por ordem convencionada
  updateCmd[5] = INFO;
  updateCmd[6] = (char)this->hourAlarmUp;
  updateCmd[7] = (char)this->minuteAlarmUp;
  updateCmd[8] = this->freqUp;
  updateCmd[9] = (char)this->hourAlarmDw;
  updateCmd[10] = (char)this->minuteAlarmDw;
  updateCmd[11] = this->freqDw;

  //restantes bytes preenchidos aleatóriamente
  byte i = 0;
  for (byte k = 12; k < 15; k++) {
    updateCmd[k] = rand() % 256;
    i += updateCmd[k];
  }
  updateCmd[15] = i;

  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)updateCmd, (byte *)updateCmd); //encriptar o comando

  //envia o comando e espera confirmação de receção
  if (deviceCommunication::sendByteArray(this, (byte *) updateCmd, DEFAULT_BYTE_ARRAY_LENGTH))
    return TRUE;
  return FALSE;
}

//Função que envia comando de desativação do dispositivo secundário
//É necessário enviar desativação quando o utilizador elimina um dispositivo secundário da lista 
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::sendDeactivation()
{
  #ifdef DEBUG
  Serial.println(F("Deactivation Sent"));
  #endif

  char deactCmd[16];
  sprintf(deactCmd, "%05u", this->id); //id
  deactCmd[5] = DISABLE; //identificação do comando
  byte i = 0;
  for (byte k = 6; k < 15; k++) {
    deactCmd[k] = rand() % 256;
    i += deactCmd[k];
  }
  deactCmd[15] = i;

  aes.clean();
  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)deactCmd, (byte *) deactCmd);// encriptação da mensagem 

  //envia o comando e espera resposta
  if (deviceCommunication::sendByteArray(this, (byte *) deactCmd, DEFAULT_BYTE_ARRAY_LENGTH))
    return TRUE;

  return FALSE;
}

//Função que verifica se um determinado dispositivo está "online"
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::test()
{
  char testCmd[16];
  sprintf(testCmd, "%05u", this->id); //id
  testCmd[5] = TEST; //identificação do comando

  byte i = 0;
  for (byte k = 6; k < 15; k++) {
    testCmd[k] = rand() % 256;
    i += testCmd[k];
  }
  testCmd[15] = i; //checksum

  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)testCmd, (byte *)testCmd); //encriptação do camando

  if (deviceCommunication::sendByteArray(this, (byte *) testCmd, DEFAULT_BYTE_ARRAY_LENGTH))
    return TRUE;
  return FALSE;
}

//Função que envia uma atualização da hora para o dispositivo secundário
//É necessário enviar desativação quando o utilizador elimina um dispositivo secundário da lista 
//Retorna TRUE se o destino recebeu com sucesso a mensagem e FALSE caso contário
byte DeviceData::sendTime()
{
  char timeCmd[16];
  sprintf(timeCmd, "%05u", this->id); //id
  timeCmd[5] = TIME; //identificação do comando
  unsigned long t = now();
  char * p = (char *)(void *)&t;
  for (byte i = 0; i < 4; i++)
    timeCmd[6 + i] = *(p + i);
  //preencher a restante mensagem aleatóriamente
  byte i = 0;
  for (byte k = 10; k < 15; k++) {
    timeCmd[k] = rand() % 256;
    i += timeCmd[k];
  }
  timeCmd[15] = i;

  #ifdef DEBUG
  for (byte j = 0; j < 16; j++)
    Serial.print(timeCmd[j]);
  Serial.println(F(" "));
  #endif

  aes.set_key (this->sharedSecret, AES_KEY_LENGTH);
  aes.encrypt ((byte *)timeCmd, (byte *)timeCmd); //encriptar a mensagem

  //envia a mensagem e aguada confirmação de receção
  if (deviceCommunication::sendByteArray(this, (byte *) timeCmd, DEFAULT_BYTE_ARRAY_LENGTH)){
    #ifdef DEBUG
    Serial.println(F("Time Sent"));
    #endif
    return TRUE;
  }

  #ifdef DEBUG
  Serial.println(F("Time update failed"));
  #endif
  return FALSE;
}

//---------------------------------- DEBUG -----------------------------
#ifdef DEBUG
void DeviceData::show()
{
  Serial.print(F("Id: "));
  Serial.flush();
  Serial.println(this->id);
  Serial.flush();
  Serial.println(this->nameDev);
  Serial.flush();
  Serial.println(this->hourAlarmUp);
  Serial.println(this->minuteAlarmUp);
  Serial.println(this->freqUp);
  Serial.println(this->hourAlarmDw);
  Serial.println(this->minuteAlarmDw);
  Serial.println(this->freqDw);
  Serial.flush();
  for (byte i = 0; i < 16; i++) {
    Serial.print(*(this->sharedSecret + i), HEX);
    Serial.print(F(" "));
  }
  Serial.println(F(" "));
  Serial.flush();
}
#endif


