/*Ficheiro MACROS_COMMUNICATION.h
 * Ficheiro onde são definidas as principais constantes necessárias para a intrepretação 
 * das mesnagens recebidas.
 * A maioria deste ficheiro é semelhante aos ficheiros homónimos da aplicação e do ccódigo do 
 * dispositivo central
 */

#ifndef MACROS_COMMUNICATION_H
#define MACROS_COMMUNICATION_H

//id único de cada dispositivo secundário
const unsigned int ID = 428;

//#define DEBUG

#define OPEN 'o'
#define CLOSE 'c'
#define TIME 't'
#define INFO 'i'
#define DISABLE 'd'
#define CONFIRMATION_MESSAGE 'r'
#define PAIRING 'p'
#define TEST 'T'

#define CODE 'c'

#define TRUE 1
#define FALSE 0

#endif

