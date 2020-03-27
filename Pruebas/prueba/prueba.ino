#include <Console.h>
#include <SPI.h>
#include <Process.h>
#include <RH_RF95.h>
#include <AESLib.h>
#include <Timer.h>

#define BAUDRATE 115200

#define RESOLUCION 0.5588
#define CAPTURAS 24 //NUMERO DE DATOS QUE SE RECIBEN PARA DETERMINAR 24H Y ENVIAR PLUVIOSIDAD (24)

//CONFIGURACIÓN DE GATEWAY
RH_RF95 rf95;
float frequency = 915.0;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //ARREGLO DE DATOS PARA RECEPCIÓN LoRa
uint8_t len = sizeof(buf); //TAMAÑO DE ARREGLO DE DATOS PARA RECEPCIÓN LoRa


uint8_t aux_env_prec = 0;
uint8_t cuenta_prec = 0;

//Timer t;

//PARA DESENCRIPTACION AES POR LoRa
uint8_t bufdato[16];
uint8_t keyLoRa[] = {1,9,9,4,1,2,0,3,1,9,9,6,0,2,1,8};


void setup() 
{
 Bridge.begin(BAUDRATE);
 //Console.begin();

  String cuenta_precAux = "";
  String timeCountAux = "";
  String EnvioBackup = "/v1.6/devices/villa-elvira/enviobackup/lv";
  String PluvioBackup = "/v1.6/devices/villa-elvira/pluviobackup/lv";

  getUbidots(&cuenta_precAux,&PluvioBackup); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE PLUVIOSIDAD
  cuenta_prec = cuenta_precAux.toInt();
  //Console.println(cuenta_prec);
  
  getUbidots(&timeCountAux,&EnvioBackup); //OBTIENE EL VALOR DE BACKUP DEL ACUMULADO DE HORAS TRANSCURRIDAS ANTES DEL PROXIMO ENVIO DE PLUVIOSIDAD
  aux_env_prec = timeCountAux.toInt();
  //Console.println(aux_env_prec);

  UbiBackup_Pluv();
  
 
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

  //t.every(60000,UbiBackup_Pluv);
}

void loop() 
{
  if (rf95.available())
  {    
    if (rf95.recv(buf, len))
    {     
    }
  }
  //t.update();
}

void getUbidots(String* dataReturn, String* Topic)
{
    //Console.println("ON");
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
    //s.addParameter("-o");
    //s.addParameter("/etc/mosquitto/value");
    s.addParameter("-C");
    s.addParameter("1");
    s.run();
    
    //PARA IMPRIMIR EN LA CONSOLA LO QUE MUESTRA LA TERMINAL AL EJECUTAR EL COMANDO DE PROCESS
    //String valor_sub = "";
    while (s.available() > 0) 
    {
      char c = s.read();
      //Console.print(c);
      *dataReturn += c;
    }
    s.flush();
     
}

void UbiBackup_Pluv()
{
  aux_env_prec++;
  String aux_data = "";
  aux_data = "{\"pluviobackup\":\"" + String(cuenta_prec);
  aux_data += "\",\"enviobackup\":\"" + String(aux_env_prec) + "\"}";
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
