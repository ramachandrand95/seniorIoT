#include <config.h>
#include <ds3231.h>

#include <SPI.h>

#include <SD.h>
#include <SdFat.h>
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

#define turbidity A1 
const int CS_PIN = 10;
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWireOne(TEMP_ONE);
OneWire oneWireTwo(TEMP_TWO);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensorOne(&oneWireOne);
DallasTemperature sensorTwo(&oneWireTwo);

//DS3231  rtc(SDA, SCL);
int totalCount = 0;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
const size_t LINE_DIM = 50;
char line[LINE_DIM];
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
  //rtc.begin();
  //rtc.setDOW(SATURDAY);
  //rtc.setTime(17, 49, 55);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(20, 1, 2018);
  pinMode(turbidity, INPUT);
//
   initializeSD();
// createFile("sample.txt");
// writeToFile("Temperature");
// closeFile();
//
//  openFile("test.txt");
  //Serial.println(readLine());
  //Serial.println(readLine());
  //closeFile();
//  configureLuxSensor();
//  sensorOne.begin();
//  sensorTwo.begin();
//  openFile("test.txt");
//  File file = SD.open("test.txt",FILE_READ);
//  while (file.available()){
//    Serial.println(readLine("test.txt"));
//  }
//  closeFile();
    readTestFile("test.txt");
    String data = "-196.60,-196.60,2272.87,65536.00,Not sent";
    writeToFile("test.txt",data);
    readTestFile("test.txt");
}
void loop() {
  
  //delay(2000);
    //sleepForFourtySeconds();
     
//    
//    String data = readLine();
//   
//    addRandomSend(data);
//    delay(5000);
    //splitString(data);
    //add 24 data log entry to the sd card
/* // add random 24 data entries
    for(int i = 0; i< 24; i++){
      String temperature_one = String(getTemp_one());
      String temperature_two = String(getTemp_two());
      String turbidity_one = String(getTurbidity());
      String luminosity_one = String(getLux());
      //String cur_time = rtc.getTimeStr();
      //String cur_date = rtc.getDateStr();
      String dataLog = temperature_one + "," + temperature_two + "," + turbidity_one + "," + luminosity_one;
      dataLog = addRandomSend(dataLog);
      writeToTest(dataLog);
    }
    Serial.println("done!");
    sleepForFourtySeconds();
*/
//    file = SD.open("test.txt",FILE_READ);
//    if(file){
//      String data = readLine();
//      Serial.println(data);
//    }
//    closeFile();
//    delay(1000);
//    Serial.println(data.endsWith("Not sent"));
//    splitString(data);
}
void readTestFile (String fileName){
  File myFile = SD.open(fileName, FILE_READ);
  String received = "";
  char ch;
  int count = 0;
  while (myFile.available())
  {
    ch = myFile.read();
    if (ch == '\n')
    {
      if(received.indexOf("Not")>0){
        Serial.println(received);
        Serial.println("need to send!");
        count++;
      }
      received = "";     
    }
    else
    {
      received += ch;
    }
  }
  Serial.println(count);
  myFile.close();
  if (SD.exists("test.txt")) {
    Serial.println("test.txt exists.");
  } else {
    Serial.println("test.txt doesn't exist.");
  }
}
void writeToFile (String fileName,String data){
  File myFile = SD.open(fileName,FILE_WRITE);
  if(myFile){
    Serial.println(fileName);
    myFile.println(data);
    Serial.println("write data to file complete");
    myFile.close();
  }
  else{
    Serial.println("error opening file");
  }
}
/*
 * the Time complexity of read line for each file is O(n)
 */
//void checkFile (){
//    
//    String dataLog = readLine();
//    int index = dataLog.lastIndexOf('Not sent');
//    Serial.print("the index of not sent is:");
//    Serial.println(index);
//    delay(1000);
//}
//void writeToTest(String dataLog){
//    file = SD.open("test.txt",FILE_WRITE);
//    Serial.println(dataLog);
//    file.println(dataLog);
//    file.close();
//}
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
  float temp1 = sensorOne.toFahrenheit(sensorOne.getTempCByIndex(0));
  return temp1;
}
float getTemp_two(){
  float temp2 = sensorTwo.toFahrenheit(sensorTwo.getTempCByIndex(0));
  return temp2;
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
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
}

//int createFile(const char filename[])
//{
//  file = SD.open(filename, FILE_WRITE);
//
//  if (file)
//  {
//    //Serial.println("File created successfully.");
//    return 1;
//  } else
//  {
//    //Serial.println("Error while creating file.");
//    return 0;
//  }
//}

//int writeToFile(String text)
//{
//  if (file)
//  {
//    file.println(text);
//    //Serial.println(text);
//    return 1;
//  } else
//  {
//    //Serial.println("Couldn't write to file");
//    return 0;
//  }
//}

//void closeFile()
//{
//  if (file)
//  {
//    file.close();
//    //Serial.println("File closed");
//  }
//}

int openFile(const char filename[])
{
  File file = SD.open(filename);
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

String readFile(String fileName)
{
  File myFile = SD.open(fileName, FILE_READ);
  String received = "";
  char ch;
  while (myFile.available())
  {
    ch = myFile.read();
    if (ch == '\n'&& received != "")
    {
      if(received.indexOf("Not")>0){
        Serial.println("need to send!");
      }
      return String(received);
      
    }
    else
    {
      received += ch;
    }
  }
  myFile.close();
  return "";
}
void sleepForFourtySeconds(){
  for(int i = 0; i<5; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF,BOD_OFF);
  }
}
void splitString (String dataLog){
   char* pch; 
   char str[30];
   dataLog.toCharArray(str,dataLog.length());
   pch = strtok (str,",");
   while(pch != NULL){
      Serial.println(pch);
      pch = strtok (NULL, ",");
   }
}
String addRandomSend(String dataLog){
   long randomNumber = random(10);
   String newData; 
   if (randomNumber < 5){
     Serial.print("the random number is: ");
     Serial.println(randomNumber);
     newData = dataLog + "," + "0";
     Serial.println(newData);
   }
   else{
     Serial.print("the random number is: ");
     Serial.println(randomNumber);
     newData = dataLog + "," + "1";
     Serial.println(newData);
   }
   return newData;
}


