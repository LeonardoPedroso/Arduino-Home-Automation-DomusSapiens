/*Ficheiro communication.h
 * Header de suporte ao ficheiro communication.cpp em que são definidas várias funções
 * necessárias para a comunicação através de rf e análise dos comandos 
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

//------------------------------- includes --------------------------------------

#include "Arduino.h"
#include <VirtualWire.h>
/*Autor: Mike McCauley
Disponível em: http://www.airspayce.com/mikem/arduino/VirtualWire/
*/
#include "MACROS_COMMUNICATION.h"
#include "alarm.h"
#include "DiffieHellman.h"
#include <TimeLib.h>
/*Autores: Michael Margolis
		   Paul Stoffregen
Disponível em: https://github.com/PaulStoffregen/Time
*/
#include <AES.h>
/*Autores: Brian Gladman
		   Karl Malbrain
		   Mark Tillotson
*/

//------------------------------ MACROS -----------------------------------------

#define DEFAULT_BYTE_ARRAY_LENGTH 17
#define AES_KEY_LENGTH 128
#define DEVICE_COMMUNICATION_TX_PIN 2
#define DEVICE_COMMUNICATION_RX_PIN 3
#define DEVICE_COMMUNICATION_BAUD_RATE 2000
#define RESPONSE_WAIT_TIMEOUT 1000

#define PAIRING_RESPONSE_WAIT_TIMEOUT 30000

//----------------------------- variáveis globais -----------------------------

//definição dos protótipos das funções no namespace communication
namespace communication
{
  void initCommunication();
  
  byte checkReception(char * cmd);
  byte checkSum(byte * byteArray, byte pos);
  void analyseCommand(char * cmd);

  void sendByteArray(byte * byteArray);
  void sendReceptionConfirmation();

  byte tryHandShakeProtocol();
  byte getCode(byte * code128bit);
  
  unsigned int power(int b, byte e);
};

#endif


