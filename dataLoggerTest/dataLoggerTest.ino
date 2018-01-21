#include <SPI.h>

#include <SD.h>

#include "LowPower.h"
#include <OneWire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
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
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
void configureLuxSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
   tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("402 ms");
  Serial.println("------------------------------------");
}
void setup() {

  Serial.begin(9600);
  pinMode(turbidity, INPUT);
//
//  initializeSD();
// createFile("sample.txt");
// writeToFile("Temperature");
// closeFile();
//
//  openFile("test.txt");
  //Serial.println(readLine());
  //Serial.println(readLine());
  //closeFile();
  configureLuxSensor();
  sensorOne.begin();
  sensorTwo.begin();
}

void loop() {
    String temperature_one = String(getTemp_one());
    String temperature_two = String(getTemp_two());
    String turbidity_one = String(getTurbidity());
    String luminosity_one = String(getLux());
    String dataLog = temperature_one + "," + temperature_two + "," + turbidity_one + "," + luminosity_one;
    writeToTest(dataLog);
}

void writeToTest(String dataLog){
    file = SD.open("test.txt",FILE_WRITE);
    Serial.println(dataLog);
    file.println(dataLog);
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
float getTemp_one(){
  float temp1[10];
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
float getTurbidity(){
  float turbidity1[10];
  for(int i = 0; i< 10; i++){
     int sensorValue = analogRead(turbidity);
     //Serial.println("turbidity sensor value: " + sensorValue);
     float voltage = sensorValue * (5.0/1024.0);
     float NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
     //Serial.println("Turbidity: " + NTU);
     turbidity1[i] = NTU;
  }
  float average = averageArray(turbidity1);
  return average;
}
float getLux(){
  float lux1[10];
  for(int i = 0; i<10; i++){
     /* Get a new sensor event */ 
    sensors_event_t event;
    tsl.getEvent(&event);
    if(event.light){
      lux1[i] = event.light;
    }
    else{
      Serial.println("Sensor overload");
    }
  }
  float average = averageArray(lux1);
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
void sleepForFourtySeconds(){
  for(int i = 0; i<5; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF,BOD_OFF);
}
}

