/*
   Ficheiro communication.cpp
   Ficheiro onde estão definidas as funções necessárias para a comunicação através de rf
   com o dispositivo central, nomeadamente no que diz respeito à receção, UVVJ;VJHVJHCJHCJHdesencriptação
   e análise de comandos. Incluir também o algoritmo para o handshake onde é estabelecido o
   shared secret entre os dois dispositivos aquando do pairing.
*/

//incluir o header
#include "communication.h"

//----------------------------- variáveis globais ------------------------------
//instância da classe AES que permitirá descodificar os comandos recebidos
AES aes;

//criar o buffer para a communicação por rf
uint8_t buf[DEFAULT_BYTE_ARRAY_LENGTH];
uint8_t buflen = DEFAULT_BYTE_ARRAY_LENGTH;

byte sharedSecret[16];

extern struct ALARM alarm;

/*
   Função que é chamada no setup e que inicia as comunicações com o dispositivo central
   e botão de pairing
*/
void communication::initCommunication(){
  getSharedSecret(); //lê o shared secret da eeprom
  pinMode(PAIRING_BUTTON_PIN, INPUT_PULLUP); //inicia o botão de pairing
  aes.set_key(sharedSecret, AES_KEY_LENGTH); //define o shared secret na instacia da classe AES
  vw_set_tx_pin(DEVICE_COMMUNICATION_TX_PIN); //define os pins para tx e rx dos módulos de rf
  vw_set_rx_pin(DEVICE_COMMUNICATION_RX_PIN);
  //vw_set_ptt_pin(DEVICE_COMMUNICATION_PTT_PIN);
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(DEVICE_COMMUNICATION_BAUD_RATE); //define a velocidade de transferência de dados
  vw_rx_start(); //inicia a receção de comandos
#ifdef DEBUG
  Serial.println(F("Device ready"));
#endif
  //define a seed para a geração de numeros aleatórios
  srand(analogRead(5));
}

/*
   Função que verifica se algum comando foi recebido, nesse caso retorna TRUE
   caso contrário retorna FALSE
   Recebe o apontador para um array de bytes (=char) para o qual copiará o comando
   recebido
*/
byte communication::checkReception(char * cmd){
  vw_rx_start(); //ALTERAR --------------------------------------------------------------
  if (vw_wait_rx_max(50) && vw_get_message(buf, &buflen)) { //verifica se neste instante alguma coisa está a ser recebida
    strcpy(cmd, (char *) buf); //copia os 16 primeiros bytes do buffer para cmd
    aes.decrypt((byte *)cmd, (byte *)cmd); //descodifica a mensagem recebida
    return TRUE;
  }
  return FALSE;
}

/*
   Por questões de seguraça todas as mensagens são enviada com alguns bytes aleatórios
   cuja suma soma mod 256 tem de ser igual ao ultimo byte do vetor.
   Esta função trata da verificação desta medida de segurança retornando TRUE se é
   verificada ou FALSE no caso contrário.
   Com o intuito que cada comando tenha o maior numero de digitos aleatórios possível,
   cada um tem um número diferente deles pelo que esta funçao recebe o apontador para o
   byte 5 do comando bem como a posição do primeiro em relaçao a esse byte
*/
byte communication::checkSum(byte * byteArray, byte pos){
  byte sum = 0; //valor da soma dos aleatórios
  //itera cada uma das posições desde pos até à posição 15 do vetor ou seja a posiçao 9 relativa ao apontador recebido
  for (byte i = pos; i < DEFAULT_BYTE_ARRAY_LENGTH - 7; i++)
    sum += byteArray[i];
  //ao somar continuamente a uma variável de 8 bits quando atinge o limite recomeça do 0 pelo que é equivalente a calcular o mod 256

  if (sum == byteArray[DEFAULT_BYTE_ARRAY_LENGTH - 7]) //verifica se a soma corresponde
    return TRUE;

  return FALSE;
}


/*
   Função que analisa o comando recebido verifica que é este o dispositivo para
   qual o comando foi enviado e  executa a respetiva ordem nessse caso.
   Esta função trata também de enviar a confirmação de receção para o dispositivo
   central para que o utilizador possa ter a certeza que o comando foi recebido e
   será executado.
*/
void communication::analyseCommand(char * cmd){
  unsigned int receivedId = 0; //variável usada para extrarir o id do comando
  for (byte i = 0; i < 5; i++) //itera os primeiros 5 bytes e gera o numero a partir da string
    receivedId += communication::power(10, 4 - i) * (cmd[i] - '0');
  if (receivedId == ID) //verifica se o destinatário da mensagem é este dispositivo
    communication::sendReceptionConfirmation(); //nesse caso envia a confirmação de que a mensagem foi recebida
  else
    return; //sai da função

  cmd += 5; //cmd passa a apontar para o byte na posição 5 uma vez que já leu os primeiros 5 bytes referentes ao id
  switch (*cmd) { //na posição 5 do vetor esta um caracter que identifica cada uma das ações
    case OPEN: {
        if (!communication::checkSum((byte *)cmd, 1)) { //verifica checksum
#ifdef DEBUG
          Serial.println(F("Check sum NOT OK"));
#endif
          break;
        }
#ifdef DEBUG
        Serial.println(F("OPEN"));
#endif
        Open();
        break;
      }
    case CLOSE: {
        if (!communication::checkSum((byte *)cmd, 1)) { //verifica checksum
          #ifdef DEBUG
          Serial.println(F("Check sum NOT OK"));
          #endif
          break;
        }
        #ifndef DEBUG
        Serial.println(F("CLOSE"));
        #endif
        Close();
        break;
      }
    case TIME: {
        if (!communication::checkSum((byte *)cmd, 5)) { //verifica checksum
          #ifdef DEBUG
          Serial.println(F("Check sum NOT OK"));
          #endif
          break;
        }
        #ifndef DEBUG
        Serial.println(F("TIME"));
        #endif
        //extrai o unsigned long dos 4 bytes a partir da posição 6
        unsigned long * timeReceived = (unsigned long *)(void *)(cmd + 1);
        setTime(*timeReceived); //atualiza a hora do sistema
        break;
      }
    case INFO: {
        if (!communication::checkSum((byte *)cmd, 7)) { //verifica checksum
         #ifdef DEBUG
          Serial.println(F("Check sum NOT OK"));
          #endif
          break;
        }
        //atualiza a informação do horário
        alarm.hourAlarmUp = (byte) cmd[1];
        alarm.minuteAlarmUp = (byte) cmd[2];
        alarm.freqUp = cmd[3];
        alarm.hourAlarmDw = (byte) cmd[4];
        alarm.minuteAlarmDw = (byte) cmd[5];
        alarm.freqDw = cmd[6];

        #ifdef DEBUG
        Serial.println(F("INFO"));
        showAlarm();
        #endif
        break;
      }
    case DISABLE: {
        if (!communication::checkSum((byte *)cmd, 1)) { //verifica checksum
           #ifdef DEBUG
          Serial.println(F("Check sum NOT OK"));
          #endif
          break;
        }
        #ifdef DEBUG
        Serial.println(F("DISABLE"));
        #endif
        //desativa alarm 
        alarm.freqUp = DISABLED;
        alarm.freqDw = DISABLED;
        break;
      }
    default: {
      #ifdef DEBUG
        Serial.println(F("NADA (PROVAVELMENTE SO UM TESTE)"));
      #endif
      }
  }
}

/*  
 * Função que envia a confirmação de que o comando enviado pelo dispositivo central foi recebido
 * e que será executada a ação corrrespondente.
 */
void communication::sendReceptionConfirmation()
{
  Serial.println("Sending confrimation");
  char confirmationMessage[6]; //array de caracteres para a mensagem
  confirmationMessage[0] = CONFIRMATION_MESSAGE; //primeiro caracter 
  sprintf(confirmationMessage + 1, "%05u", ID); //os restantes bytes são ocupados pelos digitos do id
  for(byte i = 0; i<6 ; i++){
    vw_send((byte *)confirmationMessage, DEFAULT_BYTE_ARRAY_LENGTH); //mandar a mensagem por rf 
    //vw_wait_tx(); //espera que a mesagem seja enviada
    delay(83);
  }
 
}

/*
 * Função que é chamada quando quer ser feita uma tentativa de pairing com um dispositivo central, através de um handshake e
 * troca de chaves secretas atrvés do algoritmo de Diffie_Hellman 
 * Está encarregue de enviar repetidas mensagens até obter uma resposta, a chave publica do dispositivo principal, ou até se atingir 
 * um limite temporal e o pairing ser abortado.
 * Assim que receber a chave publica do dispositivo principal gera a sua própria chave privada e a chave publica que envia para o 
 * dispositivo central. Seguidamente gera o shared secret com base na chave que recebeu do dispositivo principal que depois guarda como o
 * novo shared secret.
 */

byte communication::tryHandShakeProtocol(){
  #ifdef DEBUG
    Serial.println(F("Starting hanshake"));
  #endif

  //variável que permitirá controlar o tempo durante o qual será emitida a mensagem de pairing do dispositivo
  unsigned long start = millis(); 
  byte pairingMessage[6];//mensagem de emparelhamento
  byte code128bit[16]; //código privado de 128bits que servirá de base à troca de chaves secretas segundo o algoritmo de Diffie_Hellman
  
  pairingMessage[0] = PAIRING; //o primeiro caracter da mensagem de emparelhamento é 'p'
  sprintf((char *)(pairingMessage + 1), "%05u", ID); //os seguintes caracteres dizem respeito ao id 

  //equanto não receber uma mensagem a acusar a receção (chave publica do dispositivo central) continua a enviar mensagens de meparelhamento até a um certo limite de tempo
  while (millis() - start < PAIRING_RESPONSE_WAIT_TIMEOUT){ 
    vw_send(pairingMessage, DEFAULT_BYTE_ARRAY_LENGTH); //Manda a mensagem de emparelhamento
    if(communication::getCode(code128bit))//verifica a receção da chave publica do dispositivo central e coloca-a no vetor code128bit
      break; //se receber quebra o ciclo
  }

  if (millis() - start >= PAIRING_RESPONSE_WAIT_TIMEOUT){ //se tiverem passado o limite de tempo sem resposta retorna FALSE
    #ifdef DEBUG
    Serial.println(F("Pairing failed"));
    #endif
    return FALSE;
  }

  initAlarm();//o alarm é desativado

  #ifdef DEBUG
  Serial.println(F("Main device public key: "));
  for (byte i = 0; i < 16; i++){
    Serial.print(code128bit[i], HEX);
    Serial.print(F(" "));
    Serial.print(i);
    Serial.print(F(" "));
  } Serial.println(F(" "));
  #endif

  //gera aleatóriamente um número de 128 bits (duas variáveis de 64 bits), ie a chave secreta 
  uint64_t devicePrivate1 = randomint64();
  uint64_t devicePrivate2 = randomint64();
  //gera a chave pública do dispositibvon secundário para enviar
  uint64_t devicePublic1 = compute(G, devicePrivate1, P);
  uint64_t devicePublic2 = compute(G, devicePrivate2, P);
  
  byte codeD[17]; //vetor que servira de suporte ao envio da chave pública. 
  codeD[0] = CODE; //Tem 1 byte inicial para a indicação que a menssagem se trata de um código
  byte * p = (byte *) &devicePublic1; 
  for (byte i = 0; i < 8; i++) //preenchimento do vetor da mensagem com a chave pública
    codeD[8 - i] = *(p + i);
  p = (byte *) &devicePublic2;
  for (byte i = 0; i < 8; i++)
    codeD[16 - i] = *(p + i);

  for (byte j = 0; j < 10; j++) {
    vw_send(codeD, DEFAULT_BYTE_ARRAY_LENGTH); //envia a mensagem 6 vezes para garantir que é recebida pelo diispositivo central
    delay(50);
  }

  #ifdef DEBUG
  Serial.println(F("Public key sent"));
  /*for (byte i = 1; i <= 16; i++){
    Serial.print(codeD[i], HEX);
    Serial.print(F(" "));
  }Serial.println(F(" "));*/
  #endif

  //Nas linhas que se seguem é utilizada a variavel devicePublic apenas por forma a não utilizar outra variáve, na realidade seria mainPublic
  devicePublic1 = 0;
  for (byte i = 0; i < 8; i++)
    devicePublic1 += (uint64_t)code128bit[i] << 8*(7 - i); //extrai a primeira parte do número de 128 bits

  devicePublic2 = 0;
  for (byte i = 0; i < 8; i++)
    devicePublic2 += (uint64_t)code128bit[i + 8] << 8*(7 - i); //extrai a segunda parte do número de 128 bits

  devicePublic1 =  compute(devicePublic1, devicePrivate1, P); //gera a primeira e segunda parte do shared secret
  devicePublic2 =  compute(devicePublic2, devicePrivate2, P);

  //coloca o novo shared secret na variável global
  p = (byte *)(void *) &devicePublic1;
  for (byte i = 0; i < 8; i++)
    sharedSecret[7 - i] = *(p + i);
  p = (byte *)(void *) &devicePublic2;
  for (byte i = 0; i < 8; i++)
    sharedSecret[15 - i] = *(p + i);

  #ifdef DEBUG
  Serial.println(F("Shared secret: "));
  /*for (byte i = 0; i < 16; i++) {
    Serial.print(sharedSecret[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F(" "));*/
  #endif

  saveSharedSecret(); //guarda o sharedSecret em EEPROM
  aes.set_key(sharedSecret, AES_KEY_LENGTH);//seleciona um novo sharedSecret para a comunicação
  
  #ifdef DEBUG
  Serial.println(F("Pairing handshake successful"));
  #endif
  return TRUE;
}

/*
 * Função que envia um array de bytes e esepra pelo seu envio
 */
void communication::sendByteArray(byte * byteArray){
  vw_send(byteArray, DEFAULT_BYTE_ARRAY_LENGTH); //Envia o array de bytes por rf
  vw_wait_tx(); //Espera até a mensagem ter sido enviada
}

/*
 * Função que tenta intercetar a tranmissao de un código, recebe um array de bytes no qual inserirá o código
 * se receber um código
 */
byte communication::getCode(byte * code128bit){
  vw_rx_start(); //inicia a receção de rf
  //espera um determinado tempo pela receção de uma mensagem
  if (vw_wait_rx_max(RESPONSE_WAIT_TIMEOUT) && vw_get_message(buf, &buflen)){ 
    vw_rx_stop();
    if (buf[0] == CODE){ //verifica-se a mensagem recebida é um código
      for (byte i = 1; i <= 16; i++)
        *(code128bit + i - 1) = buf[i]; //copia o código para os endereço de memória passado por argumento
      return TRUE;
    }
  }
  vw_rx_stop();
  return FALSE;
}

/*
 * Função que calcula potencias de modo iterativo.
 * uma alternativa à função pow cujo uso de sram é excessivo para números grandes
 */
unsigned int communication::power(int b, byte e)
{
  unsigned int result = 1;
  for (byte i = 0; i < e; i++)
    result *= b;
  return result;
}




