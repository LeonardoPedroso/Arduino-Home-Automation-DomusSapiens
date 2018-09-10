/*
 * Ficheiro DiffieHellman.h
 * Header do ficheiro DiffieHellman.h
 * Define protótipos de funções e constantes necessária para a geração do shared secret
 */
#ifndef DIFFIEHELLMAN_H
#define DEFFIEHELLMAN_H
//funções de outros ficheiros usadas
#include "Arduino.h"
#include "communication.h"
#include <EEPROM.h>
//Default Arduino library under a Creative Commons Attribution-ShareAlike 3.0 License

//maior primo de 64 bits
#define P 0xffffffffffffffc5ul 
#define G 5 //constante para a geração do shared secret

//inclui sharedSecret, variável global definida em outro documento
extern byte sharedSecret[16];

//---------------------------------------- protótipos ------------------------------------
uint64_t randomint64();
uint64_t compute(uint64_t a, uint64_t m, uint64_t n);

void saveSharedSecret();
void getSharedSecret();

#endif

