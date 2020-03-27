#define RESOLUCION_PLUVIOMETRO 0.2794
#define SENSOR_PLUVIOSIDAD 3

float val_prec = 0;
uint8_t cont_aux = 0;

void Pluviometro();

void setup() {

  Serial.begin(9600);
  pinMode(SENSOR_PLUVIOSIDAD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PLUVIOSIDAD), Pluviometro, RISING);

}

void loop() {

  Serial.print("precipitacion: ");
  Serial.print(cont_aux*RESOLUCION_PLUVIOMETRO,4);
  Serial.println(" mm      ");
  
  delay(1000);

}

void Pluviometro(){

  cont_aux++;
  /*if(cont_aux==5)
  {
    val_prec = val_prec + RESOLUCION_PLUVIOMETRO;
    cont_aux = 1;
  }*/
  
}
