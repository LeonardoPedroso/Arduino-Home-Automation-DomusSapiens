/*
 * Ficheiro timeHandling.h
 * Header que serve de suporte ao ficheiro "timeHandling.cpp"
 * Estão definidos neste ficheiro os protótipos das funções que tratam da gestão do tempo
 */

#ifndef TIME_HANDLING_H
#define TIME_HANDLING_H

//--------------------------------- INCLUDES --------------------------------
#include <DS1302RTC.h>
/*Autor: Timur Maksimov
*/

#include <TimeLib.h>
/*Autores: Michael Margolis
		   Paul Stoffregen
Disponível em: https://github.com/PaulStoffregen/Time
*/

#include <EtherCard.h>
/*Autores: Jean-Claude Wippler
		   Pascal Stang
		   Guido Socher
Disponível em: https://github.com/jcw/ethercard.git
*/


#include "user_communication.h"

//----------------------------------- MACROS -------------------------------
#define DAY 1440000UL // 1 dia em segundos


//------------------------------------ PROTÓTIPOS ----------------------
void setUpRTC();
byte updateRTC();
unsigned long getNtpTime();
void checkNeedToUpdateTime();

#ifndef NO_DEBUG
void showTime();
#endif

#endif


