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
float temp1[10];
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensorOne(&oneWire1);
DallasTemperature sensorTwo(&oneWire2);
/********************************************************************/ 
void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensorOne.begin(); 
 sensorTwo.begin();
} 
void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 sensorOne.requestTemperatures(); // Send the command to get temperature reading from sensor 1
 sensorTwo.requestTemperatures(); // send the command to get temperature reading from sensor 2
/********************************************************************/
 Serial.println(F("Single Sample reading"));
 Serial.print(F("Shallow Temperature is: ")); 
 Serial.println(sensorOne.getTempCByIndex(0)); // Why "byIndex"?  
 Serial.print(F("Deep Temperature is: "));
 Serial.println(sensorTwo.getTempCByIndex(0));
 delay(1000); 
 //Serial.println("Taking 10 Samples on both sensors...");
 //Serial.print(getTemp_one()); Serial.println(" Average of Temp 1");
 //Serial.print(getTemp_two()); Serial.println(" Average of Deep Temp ");
 Serial.println("");
// printArray(temp1);
 //delay(10000);
} 
void printArray(float *array){
  for(int index =0; index < 10;index++)
  {
    Serial.println(array[index]);
  }
}

float averageArray(float *array ){
   float sum, avg;
   int loop;
   sum = avg = 0;  
   for(loop = 0; loop < 10; loop++) {
      sum = sum + array[loop];
   }
   avg = sum / loop;   
  return avg;
}

float getTemp_one(){
  for(int i = 0; i<10;i++){
    temp1[i] = sensorOne.toFahrenheit(sensorOne.getTempCByIndex(0));
  }
  float average = averageArray(temp1);
  return average;
}
float getTemp_two(){
  float temp2[10];
  for(int i = 0; i<10;i++){
    temp2[i] = sensorTwo.toFahrenheit(sensorTwo.getTempCByIndex(0));
  }
  float average = averageArray(temp2);
  return average;
}


