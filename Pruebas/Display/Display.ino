#include <printf.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <SPI.h>
#include <Event.h>
#include <Timer.h>
#include <LiquidCrystal.h>

#define NIVEL_BAT A5
#define PHSensorPin  A0    //dissolved oxygen sensor analog output pin to arduino mainboard
#define VREF 5.0    //for arduino uno, the ADC reference is the AVCC, that is 5.0V(TYP)
#define OFFSET 0.56  //zero drift compensation
#define SCOUNT  30           // sum of sample point

//PARA USO DEL pH
int analogBuffer[SCOUNT];    //store the analog value in the array, readed from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage,phValue, aux_pH;
String auxString;

//PARA FUNCION DEL TIMER
Timer t;
int stopPrinter;

//PARA MEDIDA DE LA BATERIA
int aux_bat = 0;
uint8_t porc_bat = 0;

//PARA EL DISPLAY
String textoMostrar = "!Espere!";
int i = textoMostrar.length()+1;

//PINES DE CONEXIÓN ARDUINO-DISPLAY 8x2
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8; 
//DEFINICIÓN DE PINES
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//PARA USO DEL MODULO RF
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
const uint64_t pipe = 0xE8E8F0F0E5LL;
uint8_t data[3];
bool j = 0;

void setup() 
{
  //Serial.begin(115200);
  
  pinMode(PHSensorPin,INPUT);
  pinMode(NIVEL_BAT,INPUT); //PARA LA BATERIA

  //CONFIGURACION DEL NRF24L01
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(76);
  radio.openWritingPipe(pipe);
  
  //SE ESPCIFICA DE QUE DIMENSIÓN ES EL DISPLAY
  lcd.begin(8, 2);  
  stopPrinter = t.every(1000, Printer);
  t.after(300000, pH); //5 MINUTOS (300000) CALIBRACION A 60000
}

void loop() 
{
  t.update();
}

void Printer()
{
  //PARA IMPRIMIR !ESPERE!
  i--;   
  lcd.clear();
  lcd.setCursor(i, 0);
  lcd.print(textoMostrar);
  if(i==0)
    i = textoMostrar.length()+1;
    
  //IMPRIMIR NIVEL DE BATERIA
  aux_bat = analogRead(NIVEL_BAT);
  porc_bat = map(aux_bat, 818, 1023, 0, 100); //818 = 6.72 v de bateria
  if((porc_bat == 0) || (porc_bat > 100))
  {
    lcd.clear();
    while(1)
    {
      lcd.setCursor(1, 0);
      lcd.print("CARGAR");
      lcd.setCursor(0, 1);
      lcd.print("BATERIA!");
    }
  }
   
  lcd.setCursor(0, 1);
  lcd.print("Bat:" + String(porc_bat) + "%");
  
}

void pH()
{
  t.stop(stopPrinter);

  while(1)
  {
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 30U)     //every 30 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(PHSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
      analogBufferIndex = 0;
   }
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 1000U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
      {
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      }
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the value more stable by the median filtering algorithm
      phValue = 3.5 * averageVoltage + OFFSET;


      /*Serial.print("   pH value:");
      Serial.println(phValue,2);*/

      //if(j==0)
      //{
        //lcd.noDisplay();
        data[0] = (uint8_t)phValue;//PARTE ENTERA pH
        //PARA PARTE DECIMAL
        aux_pH = (phValue - (uint8_t)phValue) * 100;
        auxString = String(aux_pH);
        data[1] = auxString.toInt(); //DECIMALES pH
        data[2] = 5; //ETIQUETA PARA pHMETRO  
        j = radio.write(data, sizeof data); //ENVIAR DATO POR RF
        //lcd.display();
      //}

      aux_bat = analogRead(NIVEL_BAT);
      porc_bat = map(aux_bat, 818, 1023, 0, 100); //818 = 6.72 v de bateria
      if((porc_bat == 0) || (porc_bat > 100))
      {
        lcd.clear();
        while(1)
        {
          lcd.setCursor(1, 0);
          lcd.print("CARGAR");
          lcd.setCursor(0, 1);
          lcd.print("BATERIA!");
        }
      }
           
      //Mostrar valor de pH en Display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("pH:" + String(phValue,2));
      
      //Mostrar valor Batería en Display
      lcd.setCursor(0, 1);
      lcd.print("Bat:" + String(porc_bat) + "%");
      lcd.display();
      
   }
  }
}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      {
        bTab[i] = bArray[i];
      }
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
        for (i = 0; i < iFilterLen - j - 1; i++) 
        {
          if (bTab[i] > bTab[i + 1]) 
          {
              bTemp = bTab[i];
              bTab[i] = bTab[i + 1];
              bTab[i + 1] = bTemp;
          }
        }
      }
      if ((iFilterLen & 1) > 0)
        bTemp = bTab[(iFilterLen - 1) / 2];
      else
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

    
