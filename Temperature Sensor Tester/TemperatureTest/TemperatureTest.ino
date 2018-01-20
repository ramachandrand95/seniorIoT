/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define TEMP_ONE 2
#define TEMP_TWO 3 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire1(TEMP_ONE); 
OneWire oneWire2(TEMP_TWO);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
/********************************************************************/ 
void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensor1.begin(); 
 sensor2.begin();
} 
void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 sensor1.requestTemperatures(); // Send the command to get temperature readings 
 sensor2.requestTemperatures();
/********************************************************************/
 Serial.print("Shallow Temperature is: "); 
 Serial.println(sensor1.toFahrenheit(sensor1.getTempCByIndex(0))); // Why "byIndex"?  
 Serial.print("Deep Temperature is: ");
 Serial.println(sensor2.toFahrenheit(sensor2.getTempCByIndex(0)));
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   delay(1000); 
} 
