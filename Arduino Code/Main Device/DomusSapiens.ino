//Incluir funções necessárias de outros ficheiros
#include "user_communication.h"
#include "timeHandling.h"
#include "deviceCommunication.h"
#include "DeviceData.h"
using namespace deviceCommunication;

//variaveis globais
byte lastAddedBlockAddr;
byte isOn = TRUE;

void setup()
{

  //ACABOU MEMORIA
  //PAIRING DE DEVICE JA CONNECTADO
  //SE NAO CONECTAR ESTAR SEMPRE A TENTAR E MOSTRAR NUM LED
 
  //Para depuração de erros
  #ifndef NO_DEBUG
  Serial.begin(230400);
  pinMode(4, INPUT_PULLUP);
  #endif


  //Tenta ligação à internet
  if (!setUpConnection()){
    #ifndef NO_DEBUG
    Serial.println(F("Connection failed"));
    #endif
    delay(1000);
  }

  //inicia o Real Time Clock Module
  setUpRTC();
  if (!updateRTC()) {
    #ifndef  NO_DEBUG
    Serial.println(F("Clock error"));
    #endif
  }
  
  deviceCommunication::initDeviceCommunication(); //inicia a comunicação por rf
  delay(1000);
  //deviceCommunication::setUpDevices(); //atualizar a informação dos dispositivos
  
}

void loop()
{
  delay(100);
  char * cmd = checkReception(); //verifica a receção de um pedido
  if(cmd) 
    analiseCommand(cmd); //analisa e executa o pedido recebido
  checkNeedToUpdateTime(); //verifica a necessidade de atualizar as horas

  //depuração de erros
  #ifndef NO_DEBUG
  if (digitalRead(4))
  {
    if (!isOn)
    {
      showTime();
      StorageManagement::showEEPROM();
      StorageManagement::clearEEPROM();
      StorageManagement::showEEPROM();
      /*showTime();
      DeviceData device(428);
      if (device.sendTime())
        Serial.println("TIME SET");
      else
        Serial.println("TIME NOT SET");*/
    }
    isOn = TRUE;
  }else
    isOn = FALSE;
#endif
}



