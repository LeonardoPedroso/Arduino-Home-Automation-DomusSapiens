/* Ficheiro StorageManagement.cpp
 * Ficheiro onde estão definidas as funções que regulam a escrita e leitura de dados
 * na EEPROM
 * A escrita de dados na EEPROM é feita em blocos na EEPROM do tamanho da classe DeviceData
 * e os endereços de escrita dos blocos de dados são geridos por um algoritmo de ware-levelling 
 * simples que garante uma utilização uniforme da EEPROM, aumentando, desta forma, a sua vida útil.
 * Para esta aplicação não era absolutamente necessário usar um algoritmo de ware-levelling, no entanto, 
 * como não é computacionalmente exigente e altamente pedagódico decidi implementá-lo
 * Assim, de cada vez que é guardada informação na EEPROM esses endereço fica guardado numa variável global, 
 * pelo que da proxima vez que for guarado um dispositivo o algoritmo guarda-o no próximo bloco livre, pelo 
 * que todos os enderecos serão enventualmente usados uniformizando o número de cilos read/write em cada bloco
 * O primeiro bloco onde é guardo um dispositivo é escolhido aleatoriamente
 * É de notar a diferenca entre apontador de um endereço na EEPROM (endereço de um byte na EEPROM) e um apontador
 * para um bloco da EEPROM (número do bloco correspondente À divisão da EEPROM em blocos do tamanho da classe DeviceDAta)
 * A função "realAddr" converte endereços de blocos para endereços na EEPROM
 */

//incluir o header
#include "StorageManagement.h"

//--------------------------------------- FUNÇÕES PÚBLICAS -------------------------------------

//Função que guarda um dispositivo em EEPROM
//Recebe o apontador para uma instância da classe DeviceData que guardará 
//Retorna TRUE se tiver sido guardado com sucesso e FALSE em caso contrário
byte StorageManagement::saveOnEEPROM(DeviceData * deviceAp)
{
  byte endReached = FALSE;
  byte blockAddr;

  //se o bloco onde foi escrito da ultima vez é o último, começa a procurar no primeiro
  if (lastAddedBlockAddr != (EEPROM.length() / BLOCK_SIZE) - 1)
    blockAddr = lastAddedBlockAddr + 1;
  else
    blockAddr = 0;

  //itera ao longo dos blocos até encontrar um vazio
  while (!StorageManagement::isEmpty(blockAddr)) {
    blockAddr++;
    if (blockAddr > EEPROM.length() / BLOCK_SIZE) {
      if (endReached)
        return FALSE; //EEPROM cheia
      else {
        blockAddr = 0;
        endReached = TRUE;
      }
    }
  }
  //se chegar a este ponto é porque há um espaço livre em memória no endereço indicado pela
  //variável blockAddr
  if (EEPROM_writeDevice(realAddr(blockAddr), deviceAp) != BLOCK_SIZE){ //guarda a informação nesse endereço
    lastAddedBlockAddr = blockAddr;
    return FALSE;
  }

  lastAddedBlockAddr = blockAddr; //atauliza a variavel que indica a posição do último bloco guardado
  return TRUE;
}

//Função que elimina um dispositivo em EEPROM
//recebe o apontador contendo pelo menos o id do dispositivo a eliminar
//Retorna TRUE se a eliminação foi feita com sucesso e FALSE caso contário
byte StorageManagement::deleteFromEEPROM(DeviceData *deviceAp)
{
  //instância de DeviceDAta temporária para fazer upload dos dados em EEPROM e verificar 
  //se o id corresponde aquele que queremos apagar
  DeviceData temp((char *)NULL); 
  byte found = FALSE;
  byte i;
  
  for (i = 0; i < (EEPROM.length() / BLOCK_SIZE); i++) { //itera ao longo da EEPROM
    if (StorageManagement::isEmpty(i)) //Se um bloco estiver vazio passa à frente
      continue;
    //caso contrário faz upload da informação nesse endereço para a instância de  DeviceData temporária
    StorageManagement::EEPROM_readDevice(StorageManagement::realAddr(i), &temp); 
    if (temp.id == deviceAp->id) { //se o id for o mesmo apagamos a informação, caso contrário continuamos a procurar
      found = TRUE;
      for(byte i = 0; i>16; i++)
        *(deviceAp->sharedSecret +i) = *(temp.sharedSecret+i); 
        //copia o shared secret do dispositivo que queremos eliminar por forma a ser possível enviar comando de desativação
      break;
    }
  }
  if (found) {
    for (byte j = 0; j < BLOCK_SIZE; j++)
      EEPROM.update(StorageManagement::realAddr(i) + j, 0); //reset ao bloco da EEPROM
    return TRUE;
  }
  return FALSE;
}

//Função lê o (deviceN)ésimo bloco com informação na EEPROM
//recebe o apontador com o objeto onde guardar a informação
//Retorna TRUE se a leitura foi feita com sucesso e FALSE caso não exista um bloco na posição pretendida
byte StorageManagement::readFromEEPROM(DeviceData * deviceAp, byte deviceN)
{

  byte blockAddr = 0;
  byte blockCount = 0;
  while (blockAddr < EEPROM.length() / BLOCK_SIZE) { //itera ao longo da EEPROM
    if (!StorageManagement::isEmpty(blockAddr))
      blockCount++;
    if (blockCount == deviceN)
      break;
    blockAddr++;
  }
  //se já iterou a EEPROM toda e já não existe o bloco na posição pedida retorna FALSE
  if (blockCount != deviceN) 
    return FALSE;
  //lê o dispositivo nesse endereço
  EEPROM_readDevice(StorageManagement::realAddr(blockAddr), deviceAp);

  //se encontar, por acaso, um bloco com informação inválida, que acontece quando o pairing falha por
  //aguma razao, elimina-o. E como é um bloco inválido terá de procuarar novamente o bloco na (deviceN)ésima posição
  if(deviceAp->hourAlarmUp == 61){ 
    deviceAp->removeFromEeprom();
    return StorageManagement::readFromEEPROM(deviceAp, deviceN);  
  }
   
  return TRUE;
}

//Função que lê o SharedSecret de um dispositivo guadado em EEPROM
//recebe o apontador contendo pelo menos o id do dispositivo do qual queremo extrair o sharedSecret
//Retorna TRUE se foi encontrado o dispositivo, e FALSE caso contrário
byte StorageManagement::getSharedSecretFromEeprom(DeviceData * deviceAp)
{
  DeviceData temp((char *)NULL); //instância auxiliar da classe DeviceData
  byte blockAddr = 0;
  byte blockCount = 0;
  while (blockAddr < EEPROM.length() / BLOCK_SIZE) {//itera ao longo da EEPROM
    //lê os blocos não vazios da EEPROM e verifica se era esse o dispositivo do qual queremos saber o sharedSecret
    if (!StorageManagement::isEmpty(blockAddr))
      EEPROM_readDevice(StorageManagement::realAddr(blockAddr), &temp);

    if(temp.id == deviceAp->id){
      for(byte i = 0; i<16; i++){
        *(deviceAp->sharedSecret +i) = *(temp.sharedSecret + i); //Faz upload do sharedSecret
      }
      return TRUE;
    }
    blockAddr++;
  }
  return FALSE;
}

//--------------------------------------- FUNÇÕES PRIVADAS -------------------------------------
//estas funções são usadas apenas com auxiliares às outras funções contidas neste ficheiro

//Função que guarda a informação de um dispositivo na EEPROM
//Recebe o endereço da EEPROM onde começar a guardar a informação
byte StorageManagement::EEPROM_writeDevice(unsigned int addr, DeviceData *deviceAp)
{
  //converte o apontador para o objeto para um apontador para um array de bytes
  const byte* p = (byte*)(void*)deviceAp; 
  byte i;
  for (i = 0; i < BLOCK_SIZE; i++)
    EEPROM.update(addr++, *p++); //guarda na EEPROM
  return i;
}

//ALTERAR RETURN DESTA FUNÇÃO E SO ESTUPIDO


//Função que lê a informação de um dispositivo na EEPROM
//Recebe o endereço da EEPROM onde começar a ler a informação
byte StorageManagement::EEPROM_readDevice(unsigned int addr, DeviceData * deviceAp)
{
  //converte o apontador para o objeto para um apontador para um array de bytes
  byte* p = (byte*)(void*)deviceAp;
  byte i;
  for (i = 0; i < BLOCK_SIZE; i++)
    *p++ = EEPROM.read(addr++); //lê da EEPROM
  return i;
}

//Função power alternativa à função recursiva
unsigned int StorageManagement::power(int b, byte e)
{
  unsigned int result = 1;
  for (byte i = 0; i < e; i++)
    result *= b;
  return result;
}

//Função que dado um endereço de um bloco o transforma num endereço da EEPROM
unsigned int StorageManagement::realAddr(byte blockAddr)
{
  return (blockAddr) * BLOCK_SIZE;
}

byte StorageManagement::isEmpty(byte blockAddr)
{
  const unsigned int realAddr = StorageManagement::realAddr(blockAddr);
  for (byte i = 0; i < BLOCK_SIZE; i++) {
    if (EEPROM.read(i + realAddr))
      return FALSE;
  }
  return TRUE;
}

//Função que inicia o algoritm de ware-levelling escolhendo aleatóriamente o endereço do bloco 
//onde comecar a escrever 
void StorageManagement::initWithWareLeveling()
{
  srand(analogRead(0)); //geração aleatória da seed dos números aleatórios
  //contrangimento dos valores obtidos para o endereço de um bloco
  lastAddedBlockAddr = rand()%(EEPROM.length()/BLOCK_SIZE); 
}

//---------------------------------- DEBUG --------------------------------
#ifdef DEBUG
void StorageManagement::clearEEPROM()
{
  for (unsigned int i = 0; i < EEPROM.length(); i++)
    EEPROM.update(i, 0);
}
void StorageManagement::showEEPROM()
{
  for (byte i = 0; i < 15; i++)
    Serial.print("-");
  for (unsigned int i = 0; i < EEPROM.length(); i++) {
    if (i % BLOCK_SIZE == 0)
      Serial.println(F(" "));
    Serial.print(i);
    Serial.print(F(":"));
    Serial.print(EEPROM.read(i));
    Serial.print(F("|"));
  }
  Serial.flush();
}
#endif



