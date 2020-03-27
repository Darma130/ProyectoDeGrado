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
#include <Process.h>
#include <RH_RF95.h>

#define RESOLUCION 0.2794
#define CAPTURAS 24 //NUMERO DE DATOS QUE SE RECIBEN PARA DETERMINAR 24H Y ENVIAR PLUVIOSIDAD (24)

//CONFIGURACIÓN DE GATEWAY
RH_RF95 rf95;
//int led = A2;
float frequency = 915.0;
uint8_t bufdato[RH_RF95_MAX_MESSAGE_LEN]; //ARREGLO DE DATOS PARA RECEPCIÓN LoRa
uint8_t len = sizeof(bufdato); //TAMAÑO DE ARREGLO DE DATOS PARA RECEPCIÓN LoRa

//PARA SABER EN QUE MOMENTO SE ENVIA EL DATO DE PRECIPITACIÓN (24H)
uint8_t aux_env_prec1 = 0; 
uint8_t aux_env_prec2 = 0;
uint8_t aux_env_prec3 = 0;
uint8_t aux_env_prec4 = 0;
uint8_t cuenta_prec = 0; //PARA LLEVAR LA CUENTA DEL TOTAL DE PRECIPITACIONES DIARIAS

void setup() 
{
  //pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  
  /*Console.begin();
  while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");*/
    
  //INICIAR SKETCH
  rf95.init(); 
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
  
  //Console.print("Listening on frequency: ");
  //Console.println(frequency);
}

void loop()
{
  
  if (rf95.available())
  {    
    if (rf95.recv(bufdato, len))
    {
      //digitalWrite(led, HIGH);  
      
      ubidots(); //ENVIO A UBIDOTS
      
      /*Console.print("Humedad: ");
      Console.print(bufdato[0]);
      Console.print("% --- Bateria: ");
      Console.print(bufdato[1]);
      Console.print("% --- Nodo: ");
      Console.print(bufdato[2]); //PARA SABER EL MODULO QUE ENVIA DATOS
      Console.print(" --- Precipitacion: ");
      Console.print(bufdato[3]);
      Console.print(" mm ");
      Console.print(" --- RSSI: ");
      Console.print(rf95.lastRssi(), DEC);
      Console.println("");*/
    } 
    /*else
    {
      Console.println("recv failed");
    }*/
  } 
}

void ubidots(){

    String aux_data = ""; // AUXILIAR PARA CONVERTIR LOS DATOS A STRING Y ENVIARLOS A UBIDOTS
        
    cuenta_prec += bufdato[3]; //PARA ACUMULAR ITERACIONES DE PRECIPITACIÓN

    switch(bufdato[2])
    {
       //CONDICIONAL PARA GARANTIZAR QUE SE CUENTE CADA HORA TENIENDO EN CUENTA CADA NODO COMO REFERENTE EN CASO DE AGOTARSE LA BATERÍA
       case 1: aux_env_prec1++;
       break;
       case 2: aux_env_prec2++;
       break;
       case 3: aux_env_prec3++;
       break;
       case 4: aux_env_prec4++;
       break;
    }

    if(bufdato[2] == 5)
    {
      if((bufdato[1]>0) && (bufdato[1]<10))
        aux_data = "{\"ph\":\"" + String(bufdato[0]) + "." + "0" + String(bufdato[1]) + "\"";
      else
        aux_data = "{\"ph\":\"" + String(bufdato[0]) + "." + String(bufdato[1]) + "\"";
    }
            
    else if(bufdato[2] == 1 || bufdato[2] == 2 || bufdato[2] == 3 || bufdato[2] == 4)
    {
      aux_data = "{\"humedad-modulo-" + String(bufdato[2]) + "\":\"" + String(bufdato[0]);
      aux_data += "\",\"bateria-modulo-" + String(bufdato[2]) + "\":\"" + String(bufdato[1]) + "\"";     
    }

     //CONDICIONAL PARA ENVIO DE PLUVIOSIDAD CADA 24H
     if(aux_env_prec1 == CAPTURAS || aux_env_prec2 == CAPTURAS || aux_env_prec3 == CAPTURAS || aux_env_prec4 == CAPTURAS) 
     { 
      
      aux_data += ",\"pluviosidad\":\"" + String(cuenta_prec*RESOLUCION, 4) + "\"}";
      
      aux_env_prec1 = 0; 
      aux_env_prec2 = 0; 
      aux_env_prec3 = 0; 
      aux_env_prec4 = 0; 
      cuenta_prec = 0;
     }
     
     else
     {     
       aux_data += "}";
     }

     // PARA COMANDO EN TERMINAL DE LINUX
     sendUbidots(&aux_data);
    
     aux_data = "";
  }

void sendUbidots(String* data){

    Process p;
    p.begin("mosquitto_pub");
    p.addParameter("-h");
    p.addParameter("things.ubidots.com"); //HOST UBIDOTS
    p.addParameter("-t");
    p.addParameter("/v1.6/devices/villa-elvira"); //DISPOSITIVOS - DEVICES
    p.addParameter("-u");
    p.addParameter("A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp"); //TOKEN
    p.addParameter("-p");
    p.addParameter("8883"); //PUERTO TLS
    p.addParameter("-q");
    p.addParameter("1"); //QoS UBIDOTS
    //p.addParameter("-d"); //MENSAJES DE DEPURACIÓN
    p.addParameter("--cafile");
    p.addParameter("/etc/mosquitto/certs/ca.crt"); //UBICACION DE CERTIFICADOS EN GATEWAY 
    p.addParameter("-m");
    p.addParameter(*data); //MENSAJE - DATOS
    //Console.println(*data);
    p.run();
    //p.close();
    p.flush();
    
    //PARA IMPRIMIR EN LA CONSOLA LO QUE MUESTRA LA TERMINAL AL EJECUTAR EL COMANDO DE PROCESS
    /*while (p.available() > 0) {
      char c = p.read();
      Console.print(c);
    }
    // Ensure the last bit of data is sent.
    Console.flush();

    Console.println(*data + "ENVIADO");*/
}
