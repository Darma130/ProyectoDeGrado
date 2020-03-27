/*
  LoRa Simple Yun Server :
  Support Devices: LG01. 
  
  Example sketch showing how to create a simple messageing server, 
  with the RH_RF95 class. RH_RF95 class does not provide for addressing or
  reliability, so you should only use RH_RF95 if you do not need the higher
  level messaging abilities.

  It is designed to work with the other example LoRa Simple Client

  User need to use the modified RadioHead library from:
  https://github.com/dragino/RadioHead

  modified 16 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/
//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines. 
//#define BAUDRATE 250000

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <AESLib.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

int led = A2;
float frequency = 915.0;

// Should be a message for us now   
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

uint8_t data[16];
uint8_t keyLoRa[] = {1,9,9,4,1,2,0,3,1,9,9,6,0,2,1,8};

void setup() 
{
  pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  Console.begin();
  while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  
  // Setup Spreading Factor (6 ~ 12)
  rf95.setSpreadingFactor(7);
  
  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setSignalBandwidth(125000);
  
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
  rf95.setCodingRate4(5);
  
  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop()
{
  if (rf95.available())
  {
    
    if (rf95.recv(buf, len))
    {
      
      Console.print("encrypted:");
      Console.print(buf[0]);
      Console.print(buf[1]);
      Console.print(buf[2]);
      Console.println(buf[3]);
      memcpy(data,buf,16);
      
      aes128_dec_single(keyLoRa, data);
      Console.print("decrypted:");
      Console.print(data[0]);
      Console.print(data[1]);
      Console.print(data[2]);
      Console.println(data[3]);
 
    }
    else
    {
      Console.println("recv failed");
    }
  }
}
