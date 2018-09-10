/*
 * Ficheiro DeviceData.h
 * Header que serve de suporte ao ficheiro "DeviceData.cpp"
 * Está neste ficheiro definida a classe DeviceData usada para a gestão mais 
 * organizada e eficiente da interação com os dipositivos secundários
 */

#ifndef DEVICE_DATA_H
#define DEVICE_DATA_H

//---------------------------------- INCLUDES ------------------------
#include "Arduino.h"
#include "MACROS_LIGACAO.h"
#include <AES.h>
/*Autores: Brian Gladman
           Karl Malbrain
           Mark Tillotson
*/

//--------------------------------- MACROS ---------------------------
#define AES_KEY_LENGTH 128

//defines para os contructor da classe
#define ID_ONLY 1
#define READ_EEPROM 2

//-------------------------------- CLASSE DeviceData ----------------
class DeviceData
{
  public:
    //constructors
    DeviceData(char * json);
    DeviceData(char * json, const byte mode);
    DeviceData(unsigned int id);

    //gestão da informação na EEPROM
    byte removeFromEeprom(); //remove info from EEPROM
    byte saveOnEeprom(); //save to EEPROM
    byte getDeviceFromEeprom(byte deviceN); //upload device 
    byte getSharedSecretFromEeprom();

    //pairing 
    byte getParingDevice();

    //Extração da informação da string em formato JSON
    void getJsonString(char * json);
    void getJsonStringParing(char * json);

    //device commands
    byte openCmd();
    byte closeCmd();
    byte test();
    byte sendUpdatedInfo();
    byte sendDeactivation();
    byte sendTime();

    //debugging 
    #ifdef DEBUG
    void show(); 
    #endif

    //variáveis públicas
    unsigned int id;
    byte hourAlarmUp;
    byte sharedSecret[16];
    
  private: //variaveis definidas como privadas e que integram 
    char nameDev[15]; 
    byte minuteAlarmUp;
    char freqUp;
    byte hourAlarmDw;
    byte minuteAlarmDw;
    char freqDw;

};

#endif


