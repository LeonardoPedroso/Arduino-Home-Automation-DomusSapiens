/*Ficheiro DiffieHellman.cpp
 * Este ficheiro incorpora as funções necessárias para a geração do sharedSecret,
 * a chave de 128 bits única para cada par persiana-dispositivo central que é definida
 * aquando da primeira comunicação entre os dois dispositivos.
 * Nesta primeira comunicação os dois dispositivos fazem uma troca pública de chaves 
 * baseada no algoritmo de Diffie Hellman.
 * Contém também funções que guardam esse sharedSeceret em EEPROM, para que após uma falha
 * de energia seja possível estabelecer comunicação com esta persiana sem ter de fazer 
 * outro pairing.
 * Desta forma, tendo estabelecido uma chave secreta todas as comunicaçãoes podem ser 
 * encriptadas para que seja substancialmente mais dificil o acesso de dispositivos não autorizadas 
 * à persiana. Esta encriptação é feita através do algoritmo AES com uma chave de 128 bits.
 * Para além desta encriptaçao todas as mensagens são enviadas com alguns bytes aleatórios 
 * cuja soma mod 256 é igual ao útimo byte. Assim, é impossivel controlar as persianas através 
 * de uma inteceção das comunicaçãoes e sua réplica. 
 */

//TOOOOOOOOOOOOOOOOOOOOOOOOTP

//incluir o header
#include "DiffieHellman.h"

//Função que retorna gera um número aleatório de 64bits
uint64_t randomint64() {
  //os número gerados pela função rand() têm 16bits pelo que é necessário gerar 4 
  uint64_t a = rand();
  uint64_t b = rand();
  uint64_t c = rand();
  uint64_t d = rand();
  //gerar um numero de 64 bits inserindo quantro blocos de 16 bits
  return a << 48 | b << 32 | c << 16 | d;
}

//Função que calcula a^n mod n
uint64_t compute(uint64_t a, uint64_t m, uint64_t n)
{
  uint64_t r;
  uint64_t y = 1;
  while (m > 0){
    r = m % 2;
    if (r == 1)
      y = (y*a) % n;
    a = a*a % n;
    m = m / 2;
  }
  return y;
}

//Função que guarda o sharedSecret em EEPROM
void saveSharedSecret()
{
  #ifdef DEBUG
  Serial.println(F("saving shared secret"));
  #endif
  /*Uma vex que este valor só será atualizado cada vez que for feita uma tentativa de pairing
   * é desnecessária a introdução de um algoritmo de ware leveling. Assim, o sharedSecret é 
   * guardado nos primeiros 16 bytes da EEPROM do Arrduino
   */
  for(byte i = 0; i<16; i++)
    EEPROM.update(i, sharedSecret[i]);
}

//Função que extrai o sharedSecret da EEPROM
void getSharedSecret()
{
  #ifdef DEBUG
  Serial.println(F("Getting sharedSecret from eeprom"));
  #endif
  for(byte i = 0; i<16; i++) //extrarir os primeiros 16 bytes
    sharedSecret[i]=EEPROM.read(i);
}





