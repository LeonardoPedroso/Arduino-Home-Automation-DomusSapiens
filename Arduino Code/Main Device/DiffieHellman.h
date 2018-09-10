/*
 * Ficheiro DiffieHellman.h
 * Header que serve de suporte ao ficheiro "DiffieHellman.cpp"
 * Este ficheiro contem os protótipos das funções e a definição de algumas
 * contantes necesssa´ias no algoritmo de Diffie Hellman
 */

//--------------------------- INCLUDES -------------------
#include "Arduino.h"

//----------------------------- MACROS -------------------
#define P 0xffffffffffffffc5ul //maior primo de 64 bits
#define G 5 //constante para a geração do shared secret

//----------------------------- FUNÇÕES ----------------
uint64_t randomint64();
uint64_t compute(uint64_t a, uint64_t m, uint64_t n);


