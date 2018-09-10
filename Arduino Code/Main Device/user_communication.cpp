/*
 * Ficheiro user_communication.cpp
 * Este ficheiro é o que contem as funções que regulam a comunicação com a app e que
 * funcionam como ponte entre o utilizador e os dispositivos
 * O Arduino funciona como um servidor que recebe requests da app contendo a ação que o
 * Arduino deve executar e envia a resposta para a app correspondente
 * O request contem a ação e informação necessária de forma codificada de caordo com uma 
 * convenção adotada
 */


#include "user_communication.h" //incluir o header

//--------------------------------- VARIÁVEIS GLOBAIS --------------------------
byte Ethernet::buffer[350]; //buffer
BufferFiller bfill;

//--------------------------------- DETALHES DO SERVIDOR  -------------------------------//
byte PASSWORD[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31}; //ethernet mac address

//----------------------------------- FUNÇÕES ---------------------------------//

//Função que inicia a ligação À internet através do ethernet module e communicação SPI
//e também inicia o algoritmo de gestão da EEPROM
//Retorna TRUE se estabelecido com sucessso e FALSE em caso contrário
byte setUpConnection()
{
  ENC28J60::initSPI(); //inicia comunicação SPI
  if (!ether.begin(sizeof Ethernet::buffer, mymac))
    return FALSE;

  if (!ether.staticSetup(myip/*, gwip*/)) //Inicia servidor de IP estático
    return FALSE;

  //predefenições necessárias ao tipo de comunicação pretendida
  ENC28J60::disableBroadcast(false);
  ENC28J60::disableMulticast();
  StorageManagement::initWithWareLeveling();
  return TRUE;
}

//Função que envia a resposta a um request feito pela aplicação na forma de JSON
void sendJson(char * json)
{ 
  bfill = ether.tcpOffset(); //apontador para o buffer
  bfill.emit_p(PSTR("$F""$S\r\n"), http_header, (word) json);//preeencher o buffer
  ether.httpServerReply(bfill.position());//enviar a resposta
  #ifdef DEBUG
  Serial.println(json);
  #endif
}

//Função que recebe como argumento TRUE ou FALSE
//Se receber TRUE envia uma resposta de HTTP OK caso contrário envia
//uma resposta de erro
void sendOK(byte ok)
{
  bfill = ether.tcpOffset(); //apontador para o buffer
  switch (ok) {
    case TRUE: {
        bfill.emit_p(PSTR("$F""$F"), http_header, OK_Json); //preencher o buffer com HTTP OK
        break;
      }
    case FALSE: {
        bfill.emit_p(PSTR("$F""$F"), http_header, NOTOK_Json); //preencher o buffer com erro
        break;
      }
  }
  ether.httpServerReply(bfill.position()); //enviar a resposta
}

//Função que verifica a receção de um request enviado pela aplicação
//Se idendificar um request executa a ação corresponde e envia a resposta
//Retorna o apontador para uma string que contem o comando propriamente dito e que será analisado por outra função
char * checkReception()
{
  word pos = ether.packetLoop(ether.packetReceive()); //verifica a receção 
  if (pos) { //se a posição no buffer da informação recebida não for NULL, ie, tiver recebido informação
    char *data = (char *) Ethernet::buffer + pos; //data aponta para o começo da informação recebida
    if (strncmp("GET /", data, 5) || data[5] > '9' || data[5] < '0') //Verifica se se trata de um request
      return NULL;
    else {
      byte passRight = TRUE;
      for (int i = 5; i < 15; i++) { //verifica a password numa posição convencionada do request
        if (data[i] - '0' != PASSWORD[i - 5]) {
          passRight = FALSE;
          break;
        }
      }

      if (!passRight)
        return NULL;
      else
        return extractCommand(data); //extrai a parte do comando relevante recebido se a password estiver correta
    }
  }

  return NULL;
}

//Função que coloca um caracter terminador no final do comando que é relevante e retorna 
//o apontador para o seu início
char * extractCommand(char * data)
{
  int n_characteres_comando = strcspn(data, " HTTP");
  data[n_characteres_comando] = '\0';
  return data + 15;
}

//Função que analisa o comando que recebe através de um apontador para uma string
//e executa a ação correspondente
byte analiseCommand(char * cmd)
{
  switch (cmd[0]) { //o primeiro bit refere-se à ação a tomar
    case DATA: { //request de informação do dispositivo na posição deviceN ocupada da EEPROM
        DeviceData device((char *)NULL);
        byte deviceN = 10 * (cmd[1] - '0') + (cmd[2] - '0'); //extrai o deviceN do dispositivo
        if (device.getDeviceFromEeprom(deviceN)) { //faz upload da informação da EEPROM
          char json[96];
          device.getJsonString(json); //concerte a informação para JSON
          sendJson(json);//envia a informação em JSON
        } else {
          //se não houver dispotivo na posição deviceN envia string JSON convencionado com esse significado
          sendJson("{\"x\":1}"); 
        }
        break;
      }
    case OPEN: { //request para abrir persiana
        DeviceData device (cmd, ID_ONLY); //extrai o id do dispositivo e coloca-o no objeto
        device.getSharedSecretFromEeprom(); //extrai o shared secret necessário a comunicação da instrução de abetura
        if (device.openCmd()) //envia comando de abertura e aguarda pela resposta
          sendOK(TRUE);//Se recebeu confirmação de receção responde com HTTP OK
        else
          sendOK(FALSE); //caso contrário envia mensagem de erro
        break;
      }
    case CLOSE: { //request para fechar persiana
        DeviceData device (cmd, ID_ONLY); //extrai o id do dispositivo e coloca-o no objeto
        device.getSharedSecretFromEeprom();//extrai o shared secret necessário a comunicação da instrução de fecho
        if (device.closeCmd())//envia comando de fecho e aguarda pela resposta
          sendOK(TRUE);//Se recebeu confirmação de receção responde com HTTP OK
        else
          sendOK(FALSE); //caso contrário envia mensagem de erro
        break;
      }
    case PARING_DEVICE:{ //request para tentar fazer pairing
        DeviceData device((char *)NULL);
        char json[14];
        
        if(!device.getParingDevice()){ 
          //se o pairing não for bem sucedido remove o dispositivo qie possa ter sido guardado em memória
          device.removeFromEeprom();
          break;
        }
        Serial.println("HERE COMM OKKKK");
        device.getJsonStringParing(json);
        sendJson(json);
        device.sendTime();
        break; //---------------------------------------------- AQUI TEM DE TER CUIDADO A ENVIAR NO CASO DE O PIARING TER FALHADO, FAZER ELSEE MANDAR ERRO
      }
    case DEVICE_DELETE: { //request para eliminar dispositivo
        DeviceData device (cmd, ID_ONLY); //extrai o id do dispositiv do comando
        device.getSharedSecretFromEeprom(); //upload do shared secret
        //falha ao desativar dispositivo pelo que não pode ser eliminado, porque continuaria a abrir e fechar e não teríamos controlo sobre ele
        if (device.sendDeactivation()) { 
          device.removeFromEeprom();//remove da EEPROM 
          sendOK(TRUE);//envia HTTP OK
        } else
          sendOK(FALSE);//envia ERRO
        break;
      }
    case DEVICE_EDIT: { //request para editar a informação de um dispositivo
        DeviceData device(cmd);//extrai informação do comando cerca dos alarmes
        device.getSharedSecretFromEeprom();//extrai o shared secret
        //só se conseguir remover o antigo adicionar o novo e enviar a atualização é que a edição é autorizada na aplicação
        if (device.removeFromEeprom() && device.saveOnEeprom() && device.sendUpdatedInfo()) 
          sendOK(TRUE);//envia HTTP OK
        else
          sendOK(FALSE);//envia ERRO
        break;
      }
    case DEVICE_ADD: { //request para adicionar um dispositivo novo
        DeviceData device(cmd);//extrai informação do comando cerca dos alarmes
        device.getSharedSecretFromEeprom();//extrai o shared secret
        //só se conseguir adicionar o novo dispositivo e enviar a atualização é que a adição é autorizada na aplicação
        if (device.saveOnEeprom() && device.sendUpdatedInfo())
          sendOK(TRUE);//envia HTTP OK
        else
          sendOK(FALSE);//envia ERRO
        break;
      }
    case DEVICE_TEST: { //request verificar se o dispositivo está comunicável
        DeviceData device(cmd, ID_ONLY); //extrai o id do dispositivo
        device.getSharedSecretFromEeprom();//faz upload do shared secret
        if (device.test()) {//faz ping ao dispositivo
          sendOK(TRUE); //envia HTTP OK
          //device.show();
          #ifndef NO_DEBUG
          Serial.println(F("Test OK"));
          Serial.flush();
          #endif
        } else {
          sendOK(FALSE);//envia ERRO
          #ifndef NO_DEBUG
          Serial.println(F("Test NOT OK"));
          Serial.flush();
          #endif
        }
        break;
      }
    case MAIN_TEST:
    {   //request para verificar a conexão da aplicação ao dispositivo central envia também o número de dispositivos guardados que estão online
        byte count = 0;
        DeviceData device((char *) NULL);
        for(byte i = 0; ; i++){
          if(!device.getDeviceFromEeprom(i))
            break;
          if(device.test())
            count++;
        }

        //cria uma string no formato JSON
        char msg[9];
        msg[0] = '{';
        msg[1] = STRING_DELIMITATOR;
        msg[2] = HTTPOK;
        msg[3] = STRING_DELIMITATOR;
        msg[4] = TWO_DOTS;
        
        if(count == 0)
          count = 61;
          
        sprintf(msg+5, "%u", count);
        if(count > 9){
           msg[7]= '}';
          msg[8] = '\0';
        }else{
          msg[6]= '}';
          msg[7]= '\0';
          msg[8]= '\0';
        }
        sendJson(msg);
        break;
      }
    default: {
         #ifndef NO_DEBUG
        Serial.println(F("NADA"));
        Serial.flush();
        #endif
        sendOK(FALSE);
      }
  }
  deleteEqualRequests(); //elimina pedidos iguais
  return TRUE;
}

//Função que elimina requests feitos ao servidor que sejam repetidos 
//Como o Arduino demora algum tempo a processar o comando e tem de esperar pela confirmação de receção 
//dos dipositivos secundários a app vai enviando requests iguais até receber uma resposta ou passar
//o tempo de timeout
//Mesmo que sejam apagados requests diferentes a app vai enviá-los novamente por falta resposta
void deleteEqualRequests()
{
  for (byte i = 0; i < 100; i++) {
    ether.packetLoop(ether.packetReceive());
    delay(1); //delay para estailidade
  }

}



