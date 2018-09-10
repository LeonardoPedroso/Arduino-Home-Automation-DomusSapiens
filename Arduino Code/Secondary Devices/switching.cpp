/*
   Ficheiro switching.cpp
   Ficheiro onde estão definidas as funções necessárias para a o controlo do estado das persianas,
   nomeadamente a verificação dos estados dos butões e funções que ativam a relé que aciona o motor das persianas.
*/

#include "switching.h"

//incluir o header
using namespace communication;

//variaveis globais
unsigned long hasBeenOnSince;
byte state;
unsigned int TIME_ON;

/*
 * Função que inicializa todos os pins necessários para o controlo das persianas, e que inicializa também as variáveis de controlo
 */
void initSwitches(){


  
  pinMode(SWITCH_UP_PIN, INPUT_PULLUP);
  pinMode(SWITCH_DW_PIN, INPUT_PULLUP);

  pinMode(RELAY_UP_PIN, OUTPUT);
  pinMode(RELAY_DW_PIN, OUTPUT);

  digitalWrite(RELAY_UP_PIN, HIGH);
  digitalWrite(RELAY_DW_PIN, HIGH);

  TIME_ON = timeOn();
  state = OFF;
}

/*
 * Função que verifica o estado de todos os butões e que evoca as funções necessarias
 */
void checkSwitches()
{
  if (!debouceSwitch(PAIRING_BUTTON_PIN)){ //se o butão de pairing for premido tenta o handshake
    tryHandShakeProtocol();
    return;
  }

  if (state == OFF) { //se estado é OFF verifica se os butoes para cima ou para baixo estão ligados
    if (!debouceSwitch(SWITCH_UP_PIN)) {
      digitalWrite(RELAY_DW_PIN, HIGH);
      delay(1);
      digitalWrite(RELAY_UP_PIN, LOW);
      hasBeenOnSince = millis(); //tempo desde que está ligado
      state = BUTTON_UP; //atualiza estado
    } else if (!debouceSwitch(SWITCH_DW_PIN)) {
      /*digitalWrite(RELAY_UP_PIN, HIGH);
      delay(1);*/ //VERIFICAR!!!!!!!!!!!! se é para por
      digitalWrite(RELAY_DW_PIN, LOW);
      hasBeenOnSince = millis(); //tempo desde que está ligado
      state = BUTTON_DW; //atualiza estado
    }
  } else {
    if (state == REMOTE_UP || state == BUTTON_UP){ //se estiver a subir verifica se já passou o tempo limite
      checkTimeOn(RELAY_UP_PIN);
      if (state == BUTTON_UP) { //verifica se o botão de subida já foi desligado, nesse caso o estado assa a OFF
        if (debouceSwitch(SWITCH_UP_PIN)) {
          digitalWrite(RELAY_UP_PIN, HIGH);
          state = OFF;
        }
      }
    } else if (state == REMOTE_DW || state == BUTTON_DW) {//se estiver a descer verifica se ká passou o tempo limite
      checkTimeOn(RELAY_DW_PIN);
      if (state == BUTTON_DW) { //verifica se o botão de descida já foi desligado, nesse caso o estado assa a OFF
        if (debouceSwitch(SWITCH_DW_PIN)) {
          digitalWrite(RELAY_DW_PIN, HIGH);
          state = OFF;
        }
      }
    } else if (state == BLOCKED) { //se o estado é bloqueado (passou o tempo limite um butão ativo)
      if (debouceSwitch(SWITCH_UP_PIN) && debouceSwitch(SWITCH_DW_PIN)) //se ambos estiverem desligados atualiza o estado para OFF
        state = OFF;
    }
  }
}

/*
 * Função que verifica se o tempo durante o qual o relé está ativo ultrapassou o limite
 * Nesse caso atualiza o estado dependendo do tipo de pedido de aberura.
 * Se o pedido foi feito remortamente através da apalicação atualiza para OFF se foi feito através de 
 * um botão o estado passa a bloqueado que só desbloqueará quando o butão que está a ser premido for desativado
 */
void checkTimeOn(byte relayPin) {
  if (millis() - hasBeenOnSince > TIME_ON) {
    digitalWrite(relayPin, HIGH);
    if (state == REMOTE_UP || state == REMOTE_DW)
      state = OFF;
    else
      state = BLOCKED;
  }
}

/*
 * Função que ativa o relé e abre a persiana
 */
void Open() {
  digitalWrite(RELAY_DW_PIN, HIGH);
  delay(1);
  digitalWrite(RELAY_UP_PIN, LOW);
  state = REMOTE_UP;
  hasBeenOnSince = millis();
}

/*
 * Função que ativa o relé e fecha a persiana
 */
void Close() {
  digitalWrite(RELAY_UP_PIN, HIGH);
  delay(1);
  digitalWrite(RELAY_DW_PIN, LOW);
  state = REMOTE_DW;
  hasBeenOnSince = millis();
}

/*
 * Função que no início do prograna com base no valor de um potenciómetro define o tempo
 * limite de subida/descida das persianas em milisegundos
 */
 
unsigned int timeOn(){
  return round(analogRead(TIME_ON_ANALOG_PIN) * 0.055 + 5) * 1000;
}

/*
 * Função que recolhe algumas amostras de um pin e retorna o valor que obteve mais vezes
 * O propósito desta função é filtar interferencias que possa haver e que podem causar um funcionamento 
 * imrevisível
 */
byte debouceSwitch(byte pin){
  byte count = 0;
  for(byte i = 0; i<DEBOUNCE_SAMPLES; i++){
    if(!digitalRead(pin))
      count++;
  }
  if(count>DEBOUNCE_SAMPLES/2) //se mais de metade das amostaras é low retorna LOW
    return LOW;
return HIGH; //caso contrário retorna HIGH
}

