#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <RH_RF95.h>

#define RESOLUCION 0.2794

RH_RF95 rf95;
float frequency = 868.0; //FRECUENCIA LoRa
 
const int pinCE = 4;
const int pinCSN = 5;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

uint8_t data;
uint8_t cuenta_prec=0;
uint8_t cont_aux=1; 

void pluviometro();

void setup()
{
   Serial.begin(9600);
   pinMode (3, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(3), pluviometro, RISING);
   radio.begin();
   radio.openReadingPipe(1,pipe);
   radio.startListening();

  //CONFIGURACIONES DE RED LoRa
   if (!rf95.init())
    Serial.println("init failed");
  // Setup ISM frequency
   rf95.setFrequency(frequency);
  // Setup Power,dBm
   rf95.setTxPower(13);

  // Setup Spreading Factor (6 ~ 12)
   rf95.setSpreadingFactor(7);
  
  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  //Lower BandWidth for longer distance.
   rf95.setSignalBandwidth(125000);
  
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
   rf95.setCodingRate4(5);
}
 
void loop()
{
   if (radio.available())
   {
      radio.read(&data, sizeof data); //RECIBE DATO POR RF
      Serial.print(data);
      
      rf95.send(&data, sizeof(data));// ENVIA DATO A GATEWAY LoRa
      rf95.waitPacketSent();//ESPERA AQUE EL DATO SEA ENVIADO
   
      rf95.send(&cuenta_prec, sizeof(cuenta_prec));// ENVIA DATO A GATEWAY LoRa
      rf95.waitPacketSent();//ESPERA AQUE EL DATO SEA 
   }

   Serial.print("  ---  Pluviosidad: ");
   Serial.print(cuenta_prec*RESOLUCION, 4);
   Serial.println(" mm");
}

void pluviometro(){

  //cont_aux++;
  //if(cont_aux==5){
    cuenta_prec++;
    //cont_aux=1;
  //}

   if (radio.available())
   {
      radio.read(&data, sizeof data); //RECIBE DATO POR RF
      Serial.print(data);

      rf95.send(&data, sizeof(data));// ENVIA DATO A GATEWAY LoRa
      rf95.waitPacketSent();//ESPERA AQUE EL DATO SEA 

      rf95.send(&cuenta_prec, sizeof(cuenta_prec));// ENVIA DATO A GATEWAY LoRa
      rf95.waitPacketSent();//ESPERA AQUE EL DATO SEA 
   }
    
}
