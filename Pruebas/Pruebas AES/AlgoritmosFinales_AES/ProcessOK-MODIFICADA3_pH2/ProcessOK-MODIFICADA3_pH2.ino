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
#include <AESLib.h>

#define RESOLUCION 0.5588
#define CAPTURAS 24 //NUMERO DE DATOS QUE SE RECIBEN PARA DETERMINAR 24H Y ENVIAR PLUVIOSIDAD (24)

//CONFIGURACIÓN DE GATEWAY
RH_RF95 rf95;
float frequency = 915.0;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //ARREGLO DE DATOS PARA RECEPCIÓN LoRa
uint8_t len = sizeof(buf); //TAMAÑO DE ARREGLO DE DATOS PARA RECEPCIÓN LoRa


uint8_t aux_env_prec1 = 0; //PARA SABER EN QUE MOMENTO SE ENVIA EL DATO DE PRECIPITACIÓN (24H)
uint8_t aux_env_prec2 = 0;
uint8_t aux_env_prec3 = 0;  
uint8_t aux_env_prec4 = 0; 
uint8_t cuenta_prec = 0; //PARA LLEVAR LA CUENTA DEL TOTAL DE PRECIPITACIONES DIARIAS

//Timer t;

//PARA DESENCRIPTACION AES POR LoRa
uint8_t bufdato[16];
uint8_t keyLoRa[] = {1,9,9,4,1,2,0,3,1,9,9,6,0,2,1,8};

void setup() 
{
  //AUXILIARES PARA BACKUP DE PLUVIOSIDAD
  String cuenta_precAux = "";
  String timeCountAux = "";
  String EnvioBackup1 = "/v1.6/devices/villa-elvira/enviobackup-1/lv";
  String EnvioBackup2 = "/v1.6/devices/villa-elvira/enviobackup-2/lv";
  String EnvioBackup3 = "/v1.6/devices/villa-elvira/enviobackup-3/lv";
  String EnvioBackup4 = "/v1.6/devices/villa-elvira/enviobackup-4/lv";
  String PluvioBackup = "/v1.6/devices/villa-elvira/pluviobackup/lv";
  
  //pinMode(led, OUTPUT);     
  Bridge.begin(BAUDRATE);
  
  /*Console.begin();
  while (!Console) ; // Wait for console port to be available
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");*/

  getUbidots(&cuenta_precAux,&PluvioBackup); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE PLUVIOSIDAD
  cuenta_prec = cuenta_precAux.toInt();
  
  getUbidots(&timeCountAux,&EnvioBackup1); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE HORAS TRANSCURRIDAS ANTES DEL PROXIMO ENVIO DE PLUVIOSIDAD
  aux_env_prec1 = timeCountAux.toInt();

  getUbidots(&timeCountAux,&EnvioBackup2); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE HORAS TRANSCURRIDAS ANTES DEL PROXIMO ENVIO DE PLUVIOSIDAD
  aux_env_prec2 = timeCountAux.toInt();

  getUbidots(&timeCountAux,&EnvioBackup3); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE HORAS TRANSCURRIDAS ANTES DEL PROXIMO ENVIO DE PLUVIOSIDAD
  aux_env_prec3 = timeCountAux.toInt();

  getUbidots(&timeCountAux,&EnvioBackup4); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE HORAS TRANSCURRIDAS ANTES DEL PROXIMO ENVIO DE PLUVIOSIDAD
  aux_env_prec4 = timeCountAux.toInt();

  UbiBackup_Pluv(&aux_env_prec1);
    
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

  //t.every(3600000,UbiBackup_Pluv);
  
  //Console.print("Listening on frequency: ");
  //Console.println(frequency);
}

void loop()
{
  
  if (rf95.available())
  {    
    if (rf95.recv(buf, len))
    {
      //digitalWrite(led, HIGH);  
      memcpy(bufdato,buf,16);
      aes128_dec_single(keyLoRa, bufdato);
      
      ubidots(); //ENVIO A UBIDOTS
      
      /*Console.print("Dato 1 ");
      Console.print(bufdato[0]);
      Console.print("% --- Dato 2 ");
      Console.print(bufdato[1]);
      Console.print("% --- Dato 3 ");
      Console.print(bufdato[2]); //PARA SABER EL MODULO QUE ENVIA DATOS
      Console.print(" --- Dato 4 ");
      Console.println(bufdato[3]);
      /*Console.print(" mm ");
      Console.print(" --- RSSI: ");
      Console.print(rf95.lastRssi(), DEC);
      Console.println("");*/
    } 
    /*else
    {
      Console.println("recv failed");
    }*/
  } 
  //t.update();
}

void ubidots(){

    String aux_data = ""; // AUXILIAR PARA CONVERTIR LOS DATOS A STRING Y ENVIARLOS A UBIDOTS
        
    cuenta_prec += bufdato[3]; //PARA ACUMULAR ITERACIONES DE PRECIPITACIÓN

    switch(bufdato[2])
    {
       //CONDICIONAL PARA GARANTIZAR QUE SE CUENTE CADA HORA TENIENDO EN CUENTA CADA NODO COMO REFERENTE EN CASO DE AGOTARSE LA BATERÍA
       case 1: aux_env_prec1++;
               UbiBackup_Pluv(&aux_env_prec1);
               break;
       case 2: aux_env_prec2++;
               UbiBackup_Pluv(&aux_env_prec2);
               break;
       case 3: aux_env_prec3++;
               UbiBackup_Pluv(&aux_env_prec3);
               break;
       case 4: aux_env_prec4++;
               UbiBackup_Pluv(&aux_env_prec4);
               break;
    }

    if(bufdato[2] == 5)
    {
      if((bufdato[1]>0) && (bufdato[1]<10))
      {
        aux_data = "{\"ph\":\"" + String(bufdato[0]) + "." + "0" + String(bufdato[1]) + "\"}";
        sendUbidots(&aux_data);
        aux_data = ""; 
      }  
      else
      {
        aux_data = "{\"ph\":\"" + String(bufdato[0]) + "." + String(bufdato[1]) + "\"}";
        sendUbidots(&aux_data);
        aux_data = "";
      }
        
    }
            
    else if(bufdato[2] == 1 || bufdato[2] == 2 || bufdato[2] == 3 || bufdato[2] == 4)
    {
      aux_data = "{\"humedad-modulo-" + String(bufdato[2]) + "\":\"" + String(bufdato[0]);
      aux_data += "\",\"bateria-modulo-" + String(bufdato[2]) + "\":\"" + String(bufdato[1]) + "\"}";
      sendUbidots(&aux_data);
      aux_data = "";     
    }

     //CONDICIONAL PARA ENVIO DE PLUVIOSIDAD CADA 24H
     if(aux_env_prec1 == CAPTURAS || aux_env_prec2 == CAPTURAS || aux_env_prec3 == CAPTURAS || aux_env_prec4 == CAPTURAS ) 
     { 
      
      aux_data = "{\"pluviosidad\":\"" + String(cuenta_prec*RESOLUCION, 4) + "\"}";
 
      sendUbidots(&aux_data);
      sendUbidots(&aux_data);

      aux_env_prec1 = 0;
      aux_env_prec2 = 0;
      aux_env_prec3 = 0;
      aux_env_prec4 = 0;
      cuenta_prec = 0;

      aux_data = "{\"enviobackup-1\":\"" + String(aux_env_prec1) + "\"}";
      sendUbidots(&aux_data);
      aux_data = "{\"enviobackup-2\":\"" + String(aux_env_prec2) + "\"}";
      sendUbidots(&aux_data);
      aux_data = "{\"enviobackup-3\":\"" + String(aux_env_prec3) + "\"}";
      sendUbidots(&aux_data);
      aux_data = "{\"enviobackup-4\":\"" + String(aux_env_prec4) + "\"}";
      sendUbidots(&aux_data);
      aux_data = "{\"pluviobackup\":\"" + String(cuenta_prec) + "\"}";
      sendUbidots(&aux_data);

      aux_data = "";
   
     }
     
     /*else
     {     
       aux_data += "}";
     }*/

     // PARA COMANDO EN TERMINAL DE LINUX
     //sendUbidots(&aux_data);
    
     //aux_data = "";
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

void getUbidots(String* dataReturn, String* Topic)
{
    Process s;
    s.begin("mosquitto_sub");
    s.addParameter("-h");
    s.addParameter("things.ubidots.com"); //HOST UBIDOTS
    s.addParameter("-t");
    s.addParameter(*Topic); //DISPOSITIVOS - DEVICES
    s.addParameter("-u");
    s.addParameter("A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp"); //TOKEN
    s.addParameter("-p");
    s.addParameter("8883"); //PUERTO TLS
    s.addParameter("-q");
    s.addParameter("1"); //QoS UBIDOTS
    s.addParameter("--cafile");
    s.addParameter("/etc/mosquitto/certs/ca.crt"); //UBICACION DE CERTIFICADOS EN GATEWAY 
    s.addParameter("-C");
    s.addParameter("1");
    s.run();
    
    //PARA IMPRIMIR EN LA CONSOLA LO QUE MUESTRA LA TERMINAL AL EJECUTAR EL COMANDO DE PROCESS
    //String valor_sub = "";
    while (s.available() > 0) 
    {
      char c = s.read();
      *dataReturn += c;
    }
    s.flush();    
}

void UbiBackup_Pluv(uint8_t* aux_env_prec)
{
  //aux_env_prec++;
  String aux_data = "";
  aux_data = "{\"pluviobackup\":\"" + String(cuenta_prec);
  //aux_data += "\",\"enviobackup-" + String(bufdato[2]) + "\":\"" + String(*aux_env_prec) + "\"}";
  aux_data += "\",\"enviobackup-1\":\"" + String(*aux_env_prec) + "\"}";
  sendUbidots(&aux_data);
  aux_data = "";
}
