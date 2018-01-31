/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 3 on the Arduino 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(3); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire1);
/********************************************************************/ 
void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 

 // Start up the library 
 sensors.begin(); 
 sensors.setResolution(12);
} 
void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 sensors.requestTemperatures(); // Send the command to get temperature reading from sensor 1
/********************************************************************/
 Serial.println(F("Single Sample reading"));
 Serial.print(F("Shallow Temperature is: ")); 
 Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"?  
 Serial.print(F("Deep Temperature is: "));
 Serial.println(sensors.getTempCByIndex(1));
 delay(500); 
} 



