/*Ficheiro alarm.h
 * Header de suporte ao ficheiro alarm.cpp em que é definida a estrutura ALARM
 * e são definidos os protótipos das funções necessárias para o controlo do alarme
 */

#ifndef ALARM_H
#define ALARM_H

//----------------------------- includes ---------------------------------------
#include "Arduino.h"
#include <TimeLib.h> 
/*Autores: Michael Margolis
		   Paul Stoffregen
Disponúvel em: https://github.com/PaulStoffregen/Time
*/
#include "switching.h"

//--------------------------- MACROS -----------------------------------------
#define DISABLED 'd'
#define EVERYDAY 'e'
#define WEEKDAYS 'w'
#define WEEKENDS 'W'

//----------------------------- estrutura ALARM ----------------------

//estrutura que guarda a informaçao do alarme do dispositivo
struct ALARM{
  byte hourAlarmUp;
  byte minuteAlarmUp;
  char freqUp;
  byte hourAlarmDw;
  byte minuteAlarmDw;
  char freqDw;
};

//--------------------------- variáveis globais ---------------------------------
extern byte sharedSecret[16];

//----------------------------- protótipos ----------------------------------
void initAlarm();
void showTime();
void showAlarm();
void checkAlarm();

#endif

