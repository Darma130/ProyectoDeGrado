#include <LowPower.h>
#include <Timer.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <AESLib.h>

#define MUESTRAS_HUM 50 //CANTIDAD DE MUESTRAS A TOMAR POR CICLO DE PROMEDIOS (50)
#define SENSOR_HUMEDAD A0 //PIN ADC PARA EL SENSOR DE HUMEDAD
#define POW_HUM 3 //ALIMENTACION SENSOR DE HUMEDAD

//PINES PARA MEDIR ESTADO DE LA BATERIA
#define ACCION_RELE 5
#define NIVEL_BAT A1

//ETIQUETA NODO
#define NODO_2 2

const int pinCE = 9; //PIN PARA TRANSMISOR RF
const int pinCSN = 10; //PIN PARA TRANSMISOR RF

RF24 radio(pinCE, pinCSN); //CONFIGURACION DE PINES PARA TRANSMISION RF
Timer t;

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0B2LL;

//VARIABLES USADAS PARA ALMACENAR Y TRANSMITIR MEDIDAS PROMEDIO DE HUMEDAD
uint8_t val_hum = 0;
int aux_hum = 0;
uint16_t aux_hum2 = 0;
uint8_t count_date = 0;
uint8_t i=0;

//VARIABLES PARA MEDIR ESTADO DE LA BATERIA
int aux_bat = 0;
uint8_t porc_bat = 0;

//CADENA DE DATOS A ENVIAR (16 BYTES DEBIDO A ENCRIPTACION AES)
uint8_t data[16]; 

//LLAVE PARA ENCRIPTACION AES POR RF
uint8_t key[] = {1,9,9,6,0,2,1,8,1,9,9,4,1,2,0,3};

void setup() {

  //Serial.begin(9600);
  pinMode(ACCION_RELE, OUTPUT);
  digitalWrite(ACCION_RELE, LOW);
  pinMode(POW_HUM, OUTPUT);
  digitalWrite(POW_HUM,LOW);
  radio.begin(); //INICIALIZAR MODULO RF
  radio.setPALevel(RF24_PA_MAX); //POTENCIA MAXIMA DEL MODULO RF
  radio.setChannel(76); //SELECCION DE CANAL DE FRECUENCIA RF
  radio.openWritingPipe(pipe); //HABILITAR CANAL DE TRANSMISION RF 
  //SE ESPERA UN TIEMPO MUERTO DE 2 MINUTOS MIENTRAS EL SENSOR SE ESTABILIZA E INICIA A ADQUIRIR MEDIDAS ALREDEDOR DE UN VALOR 
  t.after(120000, Humedad); //120000 
}

void loop() {
  t.update();
}

void Humedad()
{
  //CADA 1 MINUTO CON 12 SEGUNDOS SE TOMA UNA MEDIDA PARA OBTENER 50 DATOS EN UNA HORA
  while(1){

   digitalWrite(POW_HUM,HIGH);
   aux_hum = analogRead(SENSOR_HUMEDAD);
   val_hum = map(aux_hum,0,950,0,100);
   aux_hum2 = aux_hum2 + val_hum;
   count_date++;
   
   if(count_date == MUESTRAS_HUM) //CANTIDAD DE MEDIDAS A PROMEDIAR
   {
    aux_hum2 = aux_hum2/MUESTRAS_HUM; //SE REALIZA PROMEDIO ANTES DE ENVIAR EL DATO
    data[0]=aux_hum2;
    //MEDICION BATERIA
    digitalWrite(ACCION_RELE,HIGH);
    delay(100);
    aux_bat = analogRead(NIVEL_BAT);
    digitalWrite(ACCION_RELE,LOW);
    porc_bat = map(aux_bat,0,1023,0,100);
    data[1] = porc_bat;
    data[2] = NODO_2; 

    aes128_enc_single(key, data); //ENCRIPTACION DE LOS DATOS POR AES
    radio.write(data, sizeof data); //ENVIO DE DATOS POR RF
    
    //REINICIAR VARIABLES PARA TOMAR UN NUEVO PROMEDIO DE HUMEDAD
    aux_hum2 = 0; 
    count_date = 0; 
   }

   digitalWrite(POW_HUM,LOW);
   radio.powerDown();
   //ESTO PERMITE QUE CADA MEDIDA SE TOME APROXIMADAMENTE CADA 1 MINUTO 12 SEGUNDOS
   for(i=0;i<8;i++)
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
     
   LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); 
   radio.powerUp();
  }  
}
