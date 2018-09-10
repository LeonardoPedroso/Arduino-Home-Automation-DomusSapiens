//Ficheiro DomusSapiensDevice.ino

//incluir funções necessarias dos outros ficheiros
#include "communication.h"
#include "alarm.h"
#include "switching.h"
using namespace communication;

//comentar instrução para desativar modo de debug
#define DEBUG

void setup() {

    #ifdef DEBUG
    Serial.begin(57600);
    #endif

    initAlarm(); //fazer reset ao alarm
    communication::initCommunication(); //inicia as comunicações de rf
    initSwitches(); //inicia os butoes
}

void loop() {
    checkAlarm(); //verifica se é hora de alarme
    char cmd[16];
    if (communication::checkReception(cmd)) //verifica receção
        communication::analyseCommand(cmd);
    checkSwitches(); //verifica estado dos butoes
}

