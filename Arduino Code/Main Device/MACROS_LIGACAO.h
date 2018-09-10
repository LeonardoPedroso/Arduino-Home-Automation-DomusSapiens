/*Ficheiro MACROS_LIGACAO.h
 * Ficheiro onde são definidas as principais constantes necessárias para a intrepretação 
 * das mesnagens recebidas do utilizador atavés da aplicação
 * A maioria deste ficheiro é semelhante aos ficheiros homónimos da aplicação
 */

#ifndef MACROS_LIGACAO_H
#define MARCROS_LIGACAO_H


//#define NO_DEBUG
#define DEBUG

#define TRUE 1
#define FALSE 0

//--------------------------- APP COMMUNICATION MACROS -------------------

#define OPEN 'o'
#define CLOSE 'c'
#define DATA 'D'
#define PARING_DEVICE 'p'
#define DEVICE_DELETE 'd'
#define DEVICE_EDIT 'e'
#define DEVICE_ADD 'a'
#define MAIN_TEST 'T'
#define DEVICE_TEST 't'
#define HTTPOK 'h'

#define ID 'i'
#define NAME 'n'
#define SPACE_MARKER '&'

#define HOUR_ALARM_UP 'H'
#define MINUTE_ALARM_UP 'M'
#define FREQ_ALARM_UP 'F'

#define HOUR_ALARM_DOWN 'h'
#define MINUTE_ALARM_DOWN 'm'
#define FREQ_ALARM_DOWN 'f'

#define DISABLED 'd'
#define EVERYDAY 'e'
#define WEEKDAYS 'w'
#define WEEKENDS 'W'

#define STRING_DELIMITATOR '"'
#define TWO_DOTS ':'
#define COMA ','

//-------------------------- DEVICE COMMUNICATION MACROS ---------------------
#define TIME 't'
#define INFO 'i'
#define DISABLE 'd'
#define CONFIRMATION_MESSAGE 'r'
#define PAIRING 'p'
#define CODE 'c'
#define TEST 'T'

#endif

