#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <RH_RF95.h>

#define PIN_PLUVIOMETRO 3
#define RESOLUCION 0.2794

RH_RF95 rf95; //CREACION OBJETO TIPO RH_RF95
float frequency = 915.0; //FRECUENCIA LoRa
 
const int pinCE = 4; //PIN PARA TRANSMISOR RF
const int pinCSN = 5; //PIN PARA TRANSMISOR RF

RF24 radio(pinCE, pinCSN); ////CONFIGURACION DE PINES PARA RECEPCION RF

// LISTA DE DIRECCIONES PIPE ASIGNADAS A LOS 4 NODOS DE HUMEDAD Y AL SENSOR DE pH
const uint64_t pipe[] = {0xE8E8F0F0A1LL, 0xE8E8F0F0B2LL, 0xE8E8F0F0C3LL, 0xE8E8F0F0D4LL, 0xE8E8F0F0E5LL};

//AUXILIAR PARA MEDIR PLUVIOSIDAD
uint8_t cuenta_prec=0;
//uint8_t cont_aux=1; 

uint8_t data[3]; //ARREGLO DE RECEPCION RF
uint8_t envio[4]; //ARREGLO DE TRANSMISION LORA

void pluviometro();

void setup()
{
   Serial.begin(9600);
   //HABILITAR PIN DE INTERRUPCION
   pinMode (PIN_PLUVIOMETRO, INPUT_PULLUP);
   //CONFIGURACION DE INTERRUPCION 
   attachInterrupt(digitalPinToInterrupt(PIN_PLUVIOMETRO), pluviometro, RISING); 
   radio.begin(); //INICIALIZAR MODULO RF
   radio.setPALevel(RF24_PA_MAX); //POTENCIA MAXIMA DEL MODULO RF
   radio.setChannel(76); //SELECCION DE CANAL DE FRECUENCIA RF
   radio.openReadingPipe(1,pipe[0]); // APERTURA DE PIPE NODO 1
   radio.openReadingPipe(2,pipe[1]); // APERTURA DE PIPE NODO 2
   radio.openReadingPipe(3,pipe[2]); // APERTURA DE PIPE NODO 3
   radio.openReadingPipe(4,pipe[3]); // APERTURA DE PIPE NODO 4
   radio.openReadingPipe(5,pipe[4]); // APERTURA DE PIPE NODO 5 (pH)
   radio.startListening(); //PROGRAMAR EL RF EN MODO DE ESCUCHA

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
 
void loop()
{
   if (radio.available())
   {
      radio.read(data, sizeof data); //RECIBE DATO data POR RF
      if(data[2] == 1 || data[2] == 2 || data[2] == 3 || data[2] == 4 || data[2] == 5)
      {
        envio[0]=data[0]; //HUMEDAD DEL SUELO / PARTE ENTERA pH
        envio[1]=data[1]; //NIVEL DE BATERIA / PARTE DECIMAL pH
        envio[2]=data[2]; //ETIQUETA NODO (USADA PARA DETERMINAR A QUÉ VARIABLE DE HUMEDAD SE ENVÍA LA INFORMACIÓN EN UBIDOTS O SABER SI SE MANDA PH)
        envio[3]=cuenta_prec; //ITERACION DE PRECIPITACIONES MEDIDAS
      
        rf95.send(envio, sizeof(envio));// ENVIA DATO A GATEWAY LoRa
        rf95.waitPacketSent();//ESPERA A QUE EL DATO SEA ENVIADO

        Serial.print("pH PARTE ENTERA: ");
        Serial.print(envio[0]);
        Serial.print(" --  pH PARTE DECIMAL: ");
        Serial.print(envio[1]);
        Serial.print(" --  Nodo: ");
        Serial.print(envio[2]);
        Serial.print(" --  Precitipacion: ");
        Serial.print(envio[3]*RESOLUCION,4);
        Serial.println(" mm");

        cuenta_prec=0; //SE REINICIA DESPUÉS DE CADA ENVÍO PARA ACUMULAR LA PRECIPITACIÓN EN EL GATEWAY  
      } 
     //Serial.flush();   
   }
}

void pluviometro(){

  //cont_aux++;
  //if(cont_aux==5){
    cuenta_prec++;
    //cont_aux=1;
  //}  
  Serial.print(" --  Precitipacion: ");
  Serial.print(cuenta_prec*RESOLUCION,4);
  Serial.println(" mm");
}
