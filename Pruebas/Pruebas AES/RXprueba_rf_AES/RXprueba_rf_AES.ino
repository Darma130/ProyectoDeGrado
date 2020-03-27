#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <AESLib.h>
 
const int pinCE = 4;
const int pinCSN = 5;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe[] = {0xE8E8F0F0A1LL, 0xE8E8F0F0B2LL, 0xE8E8F0F0C3LL, 0xE8E8F0F0D4LL, 0xE8E8F0F0E5LL};
//const uint64_t pipe2 = 0xE8E8F0F0E2LL;
 
uint8_t dataMAX[20];
uint8_t data[16];
uint8_t dataRev[3];
//char data2[16];

uint8_t key[] = {1,9,9,6,0,2,1,8,1,9,9,4,1,2,0,3};
 
void setup(void)
{
   Serial.begin(9600);
   radio.begin();
   radio.setPALevel(RF24_PA_MAX); //POTENCIA MAXIMA DEL MODULO RF
   radio.setChannel(76); //SELECCION DE CANAL DE FRECUENCIA RF
   radio.openReadingPipe(1,pipe[0]);
   //radio.openReadingPipe(2,pipe2);
   radio.startListening();
}
 
void loop(void)
{
   if (radio.available())
   {
      radio.read(dataMAX, sizeof dataMAX);
      //aes128_dec_single(key, data);
      memcpy(data,dataMAX,16);
      Serial.print("encrypted:");
      Serial.print(data[0]);
      Serial.print(data[1]);
      Serial.println(data[2]);
      aes128_dec_single(key, data);
      Serial.print("decrypted:");
      dataRev[0] = data[0];
      dataRev[1] = data[1];
      dataRev[2] = data[2];
      Serial.print(dataRev[0]);
      Serial.print(dataRev[1]);
      Serial.println(dataRev[2]);
 
      //Serial.print(data);
      //Serial.print("  ------  ");
      //radio.read(data2, sizeof data2); 
      //Serial.println(data2);
      //delay(1000);
   }
}
