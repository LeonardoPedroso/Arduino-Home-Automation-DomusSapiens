/*
 * Ficheiro timeHandling.cpp
 * Este ficheiro é o que contem a definição das funções que gerem o tempo, nomeadamente as
 * que gerem a forma de guardar a informação do tempo, atualizam o tempo através da ligação 
 * à internet quando necessário
 * O tempo é atualizado a cada 24 horas
 */

//incluir o header
#include "timeHandling.h"

unsigned long lastUpdate = 0;

// Set pins:  CE(rst), IO(dat),CLK
DS1302RTC RTC(9, 8, 7);

//endereço de onde será atualizada a informação do tempo
const char website[] PROGMEM = "0.pool.ntp.org";
uint8_t ntpMyPort = 123;

//Função que inicia o Real Time Clock (RTC)
void setUpRTC()
{
  RTC.haltRTC(false);
  RTC.writeEN(false);
  setSyncProvider(RTC.get);
  setSyncInterval(5);
}
//----------------------------------- ALERTA EXPERIMENTAR GATEWAY E DNS IP PARA EVITAR DHCP
//Função que atualiza o tempo através da ligação à internet
byte updateRTC()
{  
  //Só é possível obter informação do tempo se for for fornecido 
  // Net Mask, the Gateway IP address, and the DNS Server IP address
  if (!ether.dhcpSetup()) 
    return FALSE;
  if (!ether.dnsLookup(website)) 
    return FALSE;
    
  #ifdef DEBUG
    setTime(23, 31, 30, 13, 2, 2018);
  #else
    setTime(getNtpTime());
  #endif;
  
  RTC.set(now());  //set the RTC from the system time
  if(!ether.staticSetup(myip/*, gwip*/))
    return FALSE;
  return TRUE;
}

//Função que atualiza o tempo e converte o tempo recebido da internet para o formato de
//tempo desde 1 de janeiro de 1970
//Retorna 0 se não conseguiu atualizar o tempo ou o tempo desde 1 de janeiro de 1970
//caso contrário
unsigned long getNtpTime() 
{ 
  byte count = 0;
  unsigned long timeFromNTP;   
  const unsigned long seventy_years = 2208988800UL;  
  ether.ntpRequest(ether.hisip, ntpMyPort);   
  while(true) {      
    word length = ether.packetReceive();       
    ether.packetLoop(length);       
    if(length > 0 && ether.ntpProcessAnswer(&timeFromNTP, ntpMyPort))
      return (timeFromNTP - seventy_years);
    if(count > 254) //limitar as iterações para prevenir um ciclo infinito          DEFINE AQUIIIIIIII
      return 0;
    count++;
  }
  return 0;
}

//Função que verifica se é necessário fazer a atualização do tempo do dispositivo central  MANDAR TEMPO ATUALIZADO PARA OS DISPOSITIVOS
//Nesse caso atualiza-o
void checkNeedToUpdateTime()
{
  unsigned long currentMillis = millis();
  if(currentMillis-lastUpdate > DAY){
    lastUpdate = currentMillis;
    if(!updateRTC()){
        #ifdef DEBUG
        Serial.println(F("ERROR updating time"));
        #endif
    }
    #ifdef DEBUG
    else
      Serial.println(F("Updated"));
    #endif
  }
}

#ifdef DEBUG
void showTime()
{
  time_t t = now();
  Serial.print(F("DAY: "));
  Serial.print(day(t), DEC);
  Serial.print(F("/"));
  Serial.print(month(t));
  Serial.print(F("/"));
  Serial.println(year(t), DEC);
  Serial.print(F("WEEK: "));
  Serial.println(weekday(t), DEC);
  Serial.print(F("TIME: "));
  Serial.print(hour(t), DEC);
  Serial.print(F(":"));
  Serial.print(minute(t), DEC);
  Serial.print(F(":"));
  Serial.println(second(t), DEC);
}
#endif



