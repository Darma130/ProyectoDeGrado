#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <AESLib.h>
#include <RH_RF95.h>


RH_RF95 rf95; //CREACION OBJETO TIPO RH_RF95
float frequency = 915.0; //FRECUENCIA LoRa
 
const int pinCE = 4;
const int pinCSN = 5;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe[] = {0xE8E8F0F0A1LL, 0xE8E8F0F0B2LL, 0xE8E8F0F0C3LL, 0xE8E8F0F0D4LL, 0xE8E8F0F0E5LL};
//const uint64_t pipe2 = 0xE8E8F0F0E2LL;
 
uint8_t dataMAX[20];
uint8_t data[16];
uint8_t dataRev[3];
uint8_t val = 3;
//char data2[16];

uint8_t key[] = {1,9,9,6,0,2,1,8,1,9,9,4,1,2,0,3};
uint8_t keyLoRa[] = {1,9,9,4,1,2,0,3,1,9,9,6,0,2,1,8};
 
void setup(void)
{
   Serial.begin(9600);
   radio.begin();
   radio.setPALevel(RF24_PA_MAX); //POTENCIA MAXIMA DEL MODULO RF
   radio.setChannel(76); //SELECCION DE CANAL DE FRECUENCIA RF
   radio.openReadingPipe(1,pipe[0]);
   //radio.openReadingPipe(2,pipe2);
   radio.startListening();

    //CONFIGURACIONES DE RED LoRa
   //if (!rf95.init())
    //Serial.println("init failed");
  //Inicializar LoRa
  rf95.init();
  // Setup ISM frequency
   rf95.setFrequency(frequency); //DEFINICION DE FRECUENCIA LORA
  // Setup Power,dBm
   rf95.setTxPower(23); //POTENCIA DE TRANSMISION LORA
  // Setup Spreading Factor (6 ~ 12)
   rf95.setSpreadingFactor(7);
  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  //Lower BandWidth for longer distance.
   rf95.setSignalBandwidth(125000); //ANCHO DE BANDA LORA
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
   rf95.setCodingRate4(5); //TASA DE CODIFICACION 
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
      data[3] = val;
      Serial.print(dataRev[0]);
      Serial.print(dataRev[1]);
      Serial.println(dataRev[2]);

      aes128_enc_single(keyLoRa, data);

      rf95.send(data, sizeof(data));// ENVIA DATO A GATEWAY LoRa
      rf95.waitPacketSent();//ESPERA A QUE EL DATO SEA ENVIADO
      
      //Serial.print(data);
      //Serial.print("  ------  ");
      //radio.read(data2, sizeof data2); 
      //Serial.println(data2);
      //delay(1000);
   }
}
