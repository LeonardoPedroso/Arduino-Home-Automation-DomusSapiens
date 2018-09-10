/*Ficheiro alarm.cpp
 * Ficheiro onde estão definidas as funçoes necessa´ias para o funcionamento
 * do alarme.
 */

//incluir o header
#include "alarm.h"

//------------------------- variáveis globais -------------------------------------
ALARM alarm;

//função que inicia o alarm, desativando-o
void initAlarm()
{
  alarm.hourAlarmUp = 0;
  alarm.minuteAlarmUp = 0;
  alarm.freqUp = DISABLED;

  alarm.hourAlarmDw = 0;
  alarm.minuteAlarmDw = 0;
  alarm.freqDw = DISABLED;
}

/*
 * Função que retira a hora do relógio do sistema e verifica se essa hora corresponde a algum alarme, se sim 
 * verifica a periodicidade definida e consoante o dia da semana abre/fecha a persiana ou não.
 */
void checkAlarm()
{
  time_t t = now(); //hora atual
  //verifica se está na hora de subir a persiana 
  if (alarm.freqUp != DISABLED && hour(t) == alarm.hourAlarmUp && minute(t) == alarm.minuteAlarmUp && second(t) == 0){
    if(alarm.freqUp == EVERYDAY)
        Open();
    else if(alarm.freqUp == WEEKDAYS){
        if(weekday(t)>1 && weekday(t)<=6) //verifica o dia da semana
          Open();
    }else if (weekday(t)==1 || weekday(t)==7)
      Open();
  }//verifica se está na hora de descer a persiana
  else if (alarm.freqDw != DISABLED && hour(t) == alarm.hourAlarmDw && minute(t) == alarm.minuteAlarmDw && second(t) == 0){
    if(alarm.freqDw == EVERYDAY)
        Close();
    else if(alarm.freqDw == WEEKDAYS){
        if(weekday(t)>1 && weekday(t)<=6) //verifica o dia da semana
          Close();
    }else if (weekday(t)==1 || weekday(t)==7) //verifica o dia da semana
      Close();
  }
}

//Funções de debug
#ifdef DEBUG
void showTime(){
  time_t t = now();
  Serial.print(F("Day:"));
  Serial.print(day(t), DEC);
  Serial.print(F(" Month:"));
  Serial.print(month(t));
  Serial.print(F(" Year:"));
  Serial.print(year(t), DEC);
  Serial.print(F(" Week: "));
  Serial.print(weekday(t), DEC);
  
  Serial.print(hour(t), DEC);
  Serial.print(F(":"));
  Serial.print(minute(t), DEC);
  Serial.print(F(":"));
  Serial.print(second(t), DEC);
  Serial.println(F("\n"));
}

void showAlarm(){
  Serial.println(alarm.hourAlarmUp);
  Serial.println(alarm.minuteAlarmUp);
  Serial.println(alarm.freqUp);
  Serial.println(alarm.hourAlarmDw);
  Serial.println(alarm.minuteAlarmDw);
  Serial.println(alarm.freqDw);
}
#endif


