#define RESOLUCION_PLUVIOMETRO 0.2794
#define SENSOR_HUMEDAD A0
#define SENSOR_HUMEDAD2 A1
#define SENSOR_HUMEDAD3 A2
#define SENSOR_PLUVIOSIDAD 2

float val_prec = 0;
int val_hum = 0;
int val_hum2 = 0;
int val_hum3 = 0;
int aux_hum = 0;
int aux_hum2 = 0;
int aux_hum3 = 0;
uint8_t cont_aux = 1;

void Pluviometro();

void setup() {

  Serial.begin(9600);
  pinMode(SENSOR_PLUVIOSIDAD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PLUVIOSIDAD), Pluviometro, RISING);

}

void loop() {
  aux_hum = analogRead(SENSOR_HUMEDAD);
  val_hum = map(aux_hum,0,950,0,100);
  //Serial.print("precipitacion: ");
  //Serial.print(val_prec,4);
  aux_hum2 = analogRead(SENSOR_HUMEDAD2);
  val_hum2 = map(aux_hum2,0,950,0,100);

  aux_hum3 = analogRead(SENSOR_HUMEDAD3);
  val_hum3 = map(aux_hum3,0,950,0,100);
  
  //Serial.print(" mm      ");
  Serial.print("Humedad1 (A0): ");
  Serial.print(val_hum);
  Serial.print("%");
  Serial.print(" - ");
  Serial.print(aux_hum);

  Serial.print("   |   Humedad2 (A1): ");
  Serial.print(val_hum2);
  Serial.print("%");
  Serial.print(" - ");
  Serial.print(aux_hum2);

  Serial.print("   |   Humedad3 (A2): ");
  Serial.print(val_hum3);
  Serial.print("%");
  Serial.print(" - ");
  Serial.println(aux_hum3);

  
  delay(1000);

}

void Pluviometro(){

  cont_aux++;
  if(cont_aux==5)
  {
    val_prec = val_prec + RESOLUCION_PLUVIOMETRO;
    cont_aux = 1;
  }
  
}
