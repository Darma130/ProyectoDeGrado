#include <Timer.h>

#define SENSOR_HUMEDAD A0

Timer t;

int val_hum = 0;
int aux_hum = 0;
int aux_hum2 = 0;
uint8_t count_date = 0;


void setup() {

  Serial.begin(9600);
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
   if(count_date == 10) //CANTIDAD DE MEDIDAS A PROMEDIAR
   {
    aux_hum2 = aux_hum2/10; //SE REALIZA PROMEDIO ANTES DE ENVIAR EL DATO
    Serial.print("promedio de humedad = ");
    Serial.print(aux_hum2);
    Serial.println("%");
    aux_hum2 = 0;
    count_date = 0; 
   }
}
