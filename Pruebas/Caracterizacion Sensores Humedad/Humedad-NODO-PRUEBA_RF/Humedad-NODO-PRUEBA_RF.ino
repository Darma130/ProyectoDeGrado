#include <Timer.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#define MUESTRAS_HUM 2
#define SENSOR_HUMEDAD A0 //PIN ADC PARA EL SENSOR DE HUMEDAD

const int pinCE = 9; //PIN PARA TRANSMISOR RF
const int pinCSN = 10; //PIN PARA TRANSMISOR RF

RF24 radio(pinCE, pinCSN); //CONFIGURACION DE PINES PARA TRANSMISION RF
Timer t;

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

//VARIABLES USADAS PARA ALMACENAR Y TRANSMITIR MEDIDAS PROMEDIO DE HUMEDAD
uint8_t val_hum = 0;
int aux_hum = 0;
uint8_t aux_hum2 = 0;
uint8_t count_date = 0;


void setup() {

  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe); //HABILITAR CANAL DE TRANSMISION RF
  t.after(120000, Humedad); //SE ESPERA UN TIEMPO MUERTO DE 2 MINUTOS MIENTRAS EL SENSOR SE ESTABILIZA E INICIA A ADQUIRIR MEDIDAS ALREDEDOR DE UN VALOR

}

void loop() {
  t.update();
}

void Humedad()
{
  t.every(72000, Capturar); //CADA 1 MINUTO CON 12 SEGUNDOS SE TOMA UNA MEDIDA PARA OBTENER 50 DATOS EN UNA HORA
}

void Capturar()
{
   aux_hum = analogRead(SENSOR_HUMEDAD);
   val_hum = map(aux_hum,0,950,0,100);
   aux_hum2 = aux_hum2 + val_hum;
   count_date++;
   if(count_date == MUESTRAS_HUM) //CANTIDAD DE MEDIDAS A PROMEDIAR
   {
    aux_hum2 = aux_hum2/MUESTRAS_HUM; //SE REALIZA PROMEDIO ANTES DE ENVIAR EL DATO
    radio.write(&aux_hum2, sizeof aux_hum2); //ENVIO DEL DATO POR RF
    Serial.println(aux_hum2);
    
    //REINICIAR VARIABLES PARA TOMAR UN NUEVO PROMEDIO DE HUMEDAD
    aux_hum2 = 0; 
    count_date = 0; 
   }
}
