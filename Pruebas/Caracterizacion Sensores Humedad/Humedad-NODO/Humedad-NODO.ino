#define SENSOR_HUMEDAD A0
#define POW_HUM 3

uint8_t val_hum = 0;
int aux_hum = 0;
int count_time = 0;
void setup() {

  Serial.begin(9600);
  pinMode(POW_HUM, OUTPUT);
  digitalWrite(POW_HUM,HIGH);

}

void loop() {
  aux_hum = analogRead(SENSOR_HUMEDAD);
  val_hum = map(aux_hum,0,950,0,100);
  //Serial.print(" mm      ");
  Serial.print("Humedad1 (A0): ");
  Serial.print(val_hum);
  Serial.print("%");
  Serial.print(" - ");
  Serial.println(aux_hum);
  //Serial.print(" ");
  //Serial.println(count_time);
  //count_time++;

  delay(1000);

}
