/*
  Running process using Process clas for LG01

 This sketch demonstrate how to run linux command in Arduino MCU side. 

 created 5 Jun 2013
 by Cristian Maglie
 Modified 22 May 2017
 by Dragino Tech <support@dragino.com>

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Process

 */

#include <Process.h>

//PARAMETROS DE ENVÍO A UBIDOTS
String Host_Ubidots = "things.ubidots.com";
String Dispositivo = "/v1.6/devices/villa-elvira";
String Token = "A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp";
String Certificado = "/etc/mosquitto/certs/ca.crt";
String mystring; // AUXILIAR PARA CONVERTIR LOS DATOS A STRING Y ENVIARLOS A UBIDOTS
String PUERTO_TLS = "8883";
String QoS = "1";
String aux_data = "";

uint8_t data[] = {62,90,2,34};
float pluviosidad = 0.3054;

void setup() {
  // Initialize Bridge
  Bridge.begin(115200);

  // Initialize Serial
  Console.begin();

  // Wait until a Serial Monitor is connected.
  while (!Console);

  // run various example processes
  runDeviceInfo();
  //runCpuInfo();
}

void loop() {
  // Do nothing here.
}

void runDeviceInfo() {
  // Launch "curl" command and get Arduino ascii art logo from the network
  // curl is command line program for transferring data using different internet protocols
  Process p;		// Create a process and call it "p"
  
  //p.addParameter("/etc/mosquitto/certs/");
  
  //p.addParameter("-h things.ubidots.com -t /v1.6/devices/villa-elvira -u A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp -p 8883 -q 1 -m '{pluviosidad: 90}' --cafile /etc/mosquitto/certs/ca.crt");
    
     p.begin("mosquitto_pub");
     p.addParameter("-h");
     p.addParameter("things.ubidots.com");
     p.addParameter("-t");
     p.addParameter("/v1.6/devices/villa-elvira");
     p.addParameter("-u");
     p.addParameter("A1E-ejDvvsRPaDEyvyYDYhVU4akDa0VqUp");
     p.addParameter("-p");
     p.addParameter("8883");
     p.addParameter("-q");
     p.addParameter("1");
     p.addParameter("--cafile");
     p.addParameter("/etc/mosquitto/certs/ca.crt");
     p.addParameter("-m");
     mystring = "{";
     mystring += "\"pluviosidad\":\"";
     mystring += String(pluviosidad,4);
     mystring += "\",\"humedad-modulo-1\":\"";
     mystring += String(data[0]);
     mystring += "\",\"bateria-modulo-1\":\"";
     mystring += String(data[1]);
     mystring += "\"}";
     p.addParameter(mystring);
     Console.println(mystring);
     p.run();
  /*p.begin("cat");	// Process that launch the "cat" command
  p.addParameter("/etc/banner");  // Process that launch the "cat" command
  p.run();		// Run the process and wait for its termination*/

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    char c = p.read();
    Console.print(c);
  }
  // Ensure the last bit of data is sent.
  Console.flush();
}
