void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() 
{
  float f = 6.35, k;
  String auxString;
  uint8_t aux, aux_dec;
  
  uint8_t arreglo[3];

  k = (f - (uint8_t)f) * 100;
  auxString = String(k);

  arreglo[0] = (uint8_t)f;
  arreglo[1] = auxString.toInt();
  arreglo[2] = 5;
   
 
  
  Serial.print(arreglo[0]);
  Serial.print(" ---- ");
  Serial.println(arreglo[1]);
  delay(1000);
}
