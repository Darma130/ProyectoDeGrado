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

#define RESOLUCION 0.2794
#define PUERTO_TLS 8883
#define QoS 1

//PARAMETROS DE ENVÍO A UBIDOTS
String Host_Ubidots = "things.ubidots.com";
String Dispositivo = "/v1.6/devices/villa-elvira";
String Token = "A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp";
String Certificado = "/etc/mosquitto/certs/ca.crt";

//CONFIGURACIÓN DE GATEWAY
RH_RF95 rf95;
int led = A2;
float frequency = 915.0;
uint8_t data[RH_RF95_MAX_MESSAGE_LEN]; //ARREGLO DE DATOS PARA RECEPCIÓN LoRa
uint8_t lendata = sizeof(data); //TAMAÑO DE ARREGLO DE DATOS PARA RECEPCIÓN LoRa

//PARA SABER EN QUE MOMENTO SE ENVIA EL DATO DE PRECIPITACIÓN (24H * 50 = 1200 VECES)
int aux_env_prec1 = 0; 
int aux_env_prec2 = 0;
int aux_env_prec3 = 0;
int aux_env_prec4 = 0;
uint8_t cuenta_prec = 0; //PARA LLEVAR LA CUENTA DEL TOTAL DE PRECIPITACIONES DIARIAS
float total_preci; //VARIABLE DE ENVÍO DE PRECIPITACIÓN TOTAL

void ubidots();

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
  rf95.setTxPower(23);
  
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
    
    if (rf95.recv(data, lendata))
    {
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", data, lendata);
      
      //ENVIO A UBIDOTS-------------------------------
      cuenta_prec += data[3]; //PARA ACUMULAR ITERACIONES DE PRECIPITACIÓN

      switch(data[2]){
        
       //CONDICIONAL PARA GARANTIZAR QUE SE CUENTE CADA 1:12 TENIENDO EN CUENTA CADA NODO COMO REFERENTE EN CASO DE AGOTARSE LA BATERÍA
       case 1: aux_env_prec1++;
       break;
       case 2: aux_env_prec2++;
       break;
       case 3: aux_env_prec3++;
       break;
       case 4: aux_env_prec4++;
       break;
      }


      
      
      
      
    
      //ubidots();
      
      Console.print("Humedad: ");
      Console.print(data[0]);
      Console.print("% --- Bateria: ");
      Console.print(data[1]);
      Console.print("% --- Nodo: ");
      Console.print(data[2]); //PARA SABER EL MODULO QUE ENVIA DATOS
      Console.print(" --- Precipitacion: ");
      Console.print(data[3]);
      Console.print(" mm ");
      Console.print(" --- RSSI: ");
      Console.print(rf95.lastRssi(), DEC);
      Console.println("");
    }
    
    else
    {
      Console.println("recv failed");
    }
  }
}


void ubidots(){

  
     Process p;
     p.begin("mosquitto_pub");
     p.addParameter("-h");
     p.addParameter(Host_Ubidots);
     p.addParameter("-t");
     p.addParameter(Dispositivo);
     p.addParameter("-u");
     p.addParameter(Token);
     p.addParameter("-p");
     p.addParameter(PUERTO_TLS);
     p.addParameter("-q");
     p.addParameter(QoS);
     p.addParameter("--cafile");
     p.addParameter(Certificado);
     p.addParameter("-m");

     if(aux_env_prec1 == 1200 || aux_env_prec2 == 1200 || aux_env_prec3 == 1200 || aux_env_prec4 == 1200){ //CONDICIONAL PARA ENVIO DE PLUVIOSIDAD CADA 24H
      
      p.addParameter("a");
      p.run();

      aux_env_prec1 = 0; 
      aux_env_prec2 = 0; 
      aux_env_prec3 = 0; 
      aux_env_prec4 = 0; 
      cuenta_prec = 0;
     }
     
     else{
      
     
     }
  
  }
