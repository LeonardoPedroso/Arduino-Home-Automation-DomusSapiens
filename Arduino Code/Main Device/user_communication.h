/*
 * Ficheiro user_communication.h
 * Header que serve de suporte ao ficheiro "user_communication.cpp"
 * Está neste ficheiro definida a classe DeviceData usada para a gestão mais 
 * organizada e eficiente da interação com os dipositivos secundários
 */

#ifndef USER_COMMUNICATION_H
#define USER_COMMUNICATION_H

//------------------------------- INCLUDES ---------------------------------
#include <SPI.h>
//defaut Arduino library under a Creative Commons Attribution-ShareAlike 3.0 License

#include <EtherCard.h>
/*Autores: Jean-Claude Wippler
		   Pascal Stang
		   Guido Socher
Disponível em: https://github.com/jcw/ethercard.git
*/

#include <stdlib.h>
//standard c++ library

#include "MACROS_LIGACAO.h"
#include "Arduino.h"
#include "DeviceData.h"
#include "StorageManagement.h"

using namespace StorageManagement;

//------------------------------- PROTÓTIPOS ---------------------------------
byte setUpConnection();
char * checkReception();
byte analiseCommand(char * cmd);
void sendJson(char * json);
void fillBufferWithJson(char * json);
char * extractCommand(char * data);
void deleteEqualRequests();

//------------------------------- DETALHES DO SERVIDOR  -------------------------
static byte myip[]  = {192, 168, 1, 205}; // ethernet interface ip address
static byte gwip[]  = {192, 168, 1, 1}; // gateway ip address

//------------------------------- JSON MESSAGES ---------------------------------------//

//default header
const char http_header[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: application/json;charset=utf-8\r\n"
  "Server: Arduino\r\n"
  "Retry-After: 600\r\n"
  "Connnection: close\r\n\r\n";

const char OK_Json[] PROGMEM = "{\"h\":1}\r\n";
const char NOTOK_Json[] PROGMEM = "{\"h\":0}\r\n";
const char PROGMEM END_OF_TRANSMITION[] = "{\"x\":1}\r\n";


#endif

