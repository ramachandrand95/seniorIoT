#include <SPI.h>

#include <SD.h>

#include "LowPower.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
const int temp_sensor = 7;
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
// Data wire is plugged into pin 2 on the Arduino
#define TEMP_ONE 8
#define TEMP_TWO 9
#define photoCell A0
#define turbidity A1 
const int CS_PIN = 10;
File file;
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWireOne(TEMP_ONE);
OneWire oneWireTwo(TEMP_TWO);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensorOne(&oneWireOne);
DallasTemperature sensorTwo(&oneWireTwo);

typedef enum state {SLEEP,WAKEUP,SAMPLE,STORE,TRANSMIT}State;
State cur_state = STORE;
int hourCount = 0;
float sensorOne_temps[10];
float sensorTwo_temps[10];
void setup() {

  Serial.begin(9600);

  initializeSD();
 createFile("sample.txt");
 writeToFile("Temperature");
 closeFile();

  openFile("test.txt");
  //Serial.println(readLine());
  //Serial.println(readLine());
  //closeFile();

  sensorOne.begin();
  sensorTwo.begin();
}

void loop() {
//  switch(cur_state){
//    case SLEEP:
//    Serial.println("Sleep starts!");
//    //sleepForOneHour();
//    cur_state = SAMPLE;
//    break;
//    case WAKEUP:
//    break;
//    case SAMPLE:
//    Serial.println(sampleTemps());
//    delay(500);
//    cur_state = STORE;
//    break;
//    case STORE:
//    
////    writeToFile(temp_one);
//    break;
//    case TRANSMIT:
//    break;
//  }
    String temperature_one = String(averageTenTemps(sensorOne_temps));
    String temperature_two = String(averageTenTemps(sensorTwo_temps));
    String temperature = temperature_one + "," + temperature_two;
    writeToTest(temperature);
}
void writeToTest(String temperature){
    file = SD.open("test.txt",FILE_WRITE);
    Serial.println(temperature);
    file.println(temperature);
    file.close();
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
float averageTenTemps(float* tempArray){
  for(int i = 0; i<10;i++){
    tempArray[i] = sensorOne.toFahrenheit(sensorOne.getTempCByIndex(0));
  }
  float average = averageArray(tempArray);
  return average;
}
void initializeSD()
{
  Serial.println("Initializing SD card...");
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN,HIGH);
  if (SD.begin())
  {
    //Serial.println("SD card is ready to use.");
  } else
  {
    //Serial.println("SD card initialization failed");
    return;
  }
}

int createFile(const char filename[])
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    //Serial.println("File created successfully.");
    return 1;
  } else
  {
    //Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(String text)
{
  if (file)
  {
    file.println(text);
    //Serial.println(text);
    return 1;
  } else
  {
    //Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile()
{
  if (file)
  {
    file.close();
    //Serial.println("File closed");
  }
}

int openFile(const char filename[])
{
  file = SD.open(filename);
  if (file)
  {
    //Serial.println("File opened with success!");
    return 1;
  } else
  {
    //Serial.println("Error opening file...");
    return 0;
  }
}

String readLine()
{
  String received = "";
  char ch;
  while (file.available())
  {
    ch = file.read();
    if (ch == '\n')
    {
      return String(received);
    }
    else
    {
      received += ch;
    }
  }
  return "";
}
void sleepForOneHour(){
  for(int i = 0; i<450; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF,BOD_OFF);
}
}

