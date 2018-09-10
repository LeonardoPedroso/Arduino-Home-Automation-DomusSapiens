/*
 * Ficheiro deviceCommunication.h
 * Header que serve de suporte ao ficheiro " deviceCommunication.cpp"
 * Estão neste ficheir definidos os protótipos das funções que tratam da comunicação com
 * os dispositivos secundários, incluidas no namespace deviceCommunication
 */

#ifndef DEVICE_COMMUNICATION_H
#define DEVICE_COMMUNICATION_H

//--------------------------------- INCLUDES------------------------------------
#include "Arduino.h"
#include "DeviceData.h"
#include "MACROS_LIGACAO.h"
#include <VirtualWire.h>
/*Autor: Mike McCauley
Disponível em: http://www.airspayce.com/mikem/arduino/VirtualWire/
*/

//-------------------------------- MACROS -------------------------------------
#define DEVICE_COMMUNICATION_TX_PIN 2
#define DEVICE_COMMUNICATION_RX_PIN 3
#define DEVICE_COMMUNICATION_BAUD_RATE 2000

#define BUFFER_LENGTH 18
#define RESPONSE_WAIT_TIMEOUT 600
#define RESPONSE_WAIT_TIMEOUT_RECEPTION 200
#define DEFAULT_BYTE_ARRAY_LENGTH 17

//--------------------------------- PROTÓTIPOS --------------------------------
namespace deviceCommunication
{
void initDeviceCommunication();
void setUpDevices();
byte sendByteArray(DeviceData * deviceAp, byte * byteArray, byte byteArrayLen);
byte checkReception(unsigned int id);
byte handshakeProtocol(DeviceData * device);
byte getCode(byte * code128bit);
};

#endif



