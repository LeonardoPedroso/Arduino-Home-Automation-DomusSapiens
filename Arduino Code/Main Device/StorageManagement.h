/*Ficheiro StorageManagement.h
 * Header de suporte ao ficheiro StorageManagement.cpp
 * A escrita de dados na EEPROM é feita em blocos na EEPROM do tamanho da classe DeviceData
 * e os endereços de escrita dos blocos de dados são geridos por um algoritmo de ware-levelling 
 * simples que garante uma utilização uniforme da EEPROM, aumentando, desta forma, a sua vida útil.
 * Para esta aplicação não era absolutamente necessário usar um algoritmo de ware-levelling, no entanto, 
 * como não é computacionalmente exigente e altamente pedagódico decidi implementá-lo
 */
 
#ifndef STORAGE_MANAGEMENT_H
#define STORAGE_MANAGEMENT_H

//---------------------------------- INCLUDES -----------------------------
#include "Arduino.h"
#include "DeviceData.h"
#include "MACROS_LIGACAO.h"
#include <EEPROM.h>
//Default Arduino library under a Creative Commons Attribution-ShareAlike 3.0 License

#include <TimeLib.h>
/*Autores: Michael Margolis
		   Paul Stoffregen
Disponível em: https://github.com/PaulStoffregen/Time
*/
#include "DiffieHellman.h"

//-------------------------------- MACROS ---------------------------------
#define BLOCK_SIZE sizeof(DeviceData) //em bytes


//-------------------------------- VAR GLOBAIS ---------------------------------
//guarda a posição do ultimo bloco em que foi escrita informação
extern byte lastAddedBlockAddr; 

namespace StorageManagement
{
  
unsigned int power(int b, byte e);

#ifdef DEBUG
void showEEPROM();
void clearEEPROM();
#endif

void initWithWareLeveling();

byte saveOnEEPROM(DeviceData *deviceAp);
byte deleteFromEEPROM(DeviceData *deviceAp);
byte readFromEEPROM(DeviceData * deviceAp, byte deviceN);
byte getSharedSecretFromEeprom(DeviceData * deviceAp);

//private
byte EEPROM_writeDevice(unsigned int addr, DeviceData * deviceAp);
byte EEPROM_readDevice(unsigned int addr, DeviceData * deviceAp);
byte isEmpty(byte blockReal);
unsigned int realAddr(byte blockAddr);
};

#endif


