#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <AESLib.h>
 
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0A1LL;
 
uint8_t key[] = {1,9,9,6,0,2,1,8,1,9,9,4,1,2,0,3};
uint8_t data[16] = {0,1,2}; //16 chars == 16 bytes
 
void setup(void)
{
   radio.begin();
   radio.openWritingPipe(pipe);
   aes128_enc_single(key, data);
}
 
void loop(void)
{
   radio.write(&data, sizeof data);
   delay(1000);
}
