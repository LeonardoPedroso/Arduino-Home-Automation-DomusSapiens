/*Ficheiro switching.h
 * Header de suporte ao ficheiro switching.cpp em que são definidas várias funções
 * necessárias para a o controlo de abertura e fecho das persianas, bem como de os botões
 * que também as controlam
 */
#ifndef SWITCHING_H
#define SWITCHING_H

//----------------------------- includes ---------------------------------------
#include "alarm.h"
#include "communication.h"

//----------------------------- MACROS ---------------------------------------
#define SWITCH_UP_PIN 7
#define SWITCH_DW_PIN 8

#define RELAY_UP_PIN 5
#define RELAY_DW_PIN 6

#define PAIRING_BUTTON_PIN 4

#define TIME_ON_ANALOG_PIN A0

#define OFF 0
#define REMOTE_UP 1
#define REMOTE_DW 2
#define BUTTON_UP 3
#define BUTTON_DW 4

#define BLOCKED 5

#define DEBOUNCE_SAMPLES 10


//----------------------------- protótipos ---------------------------------------
void checkSwitches();
void initSwitches();

void checkTimeOn(byte relayPin);

void Open();
void Close();

unsigned int timeOn();

byte debouceSwitch(byte pin);

#endif
