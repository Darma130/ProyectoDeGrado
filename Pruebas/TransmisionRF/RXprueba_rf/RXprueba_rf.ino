#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
 
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint64_t pipe2 = 0xE8E8F0F0E2LL;
 
char data[16];
char data2[16];
 
void setup(void)
{
   Serial.begin(9600);
   radio.begin();
   radio.openReadingPipe(1,pipe);
   radio.openReadingPipe(2,pipe2);
   radio.startListening();
}
 
void loop(void)
{
   if (radio.available())
   {
      radio.read(data, sizeof data); 
      Serial.print(data);
      Serial.print("  ------  ");
      radio.read(data2, sizeof data2); 
      Serial.println(data2);
      delay(1000);
   }
}
