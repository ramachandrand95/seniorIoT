#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <OneWire.h> 
#include <LowPower.h>
#include <DallasTemperature.h>
#include <DS3231.h>
#include <LiquidCrystal.h> 
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
// Data wire is plugged into pin 2 on the Arduino
#define TEMP_ONE 2
#define TEMP_TWO 3
#define turbidity A0

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWireOne(TEMP_ONE);
OneWire oneWireTwo(TEMP_TWO);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensorsOne(&oneWireOne);
DallasTemperature sensorsTwo(&oneWireTwo);

DS3231  rtc(SDA, SCL);
//LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
char data[50];

//setting up xbee transmit and receive pins.
uint8_t xbee_recv = 2; 
uint8_t xbee_trans = 4; 
SoftwareSerial xbee(xbee_recv, xbee_trans);
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
 
void setup(void)
{
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  rtc.begin(); // Initialize the rtc object
    
  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(12, 02, 55);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(14, 1, 2018);
  pinMode(turbidity, INPUT);

  // start serial port
  Serial.begin(9600);

  // Start up temperature sensor library
  sensorsOne.begin();
  sensorsTwo.begin();
  
   Serial.println(F("Light Sensor Test")); Serial.println(F(""));
   /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print(F("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!"));
    while(1);
  }
  /* Display some basic information on this sensor */
  sensor_t sensor;
  tsl.getSensor(&sensor);
  
  /* Setup the sensor gain and integration time */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  //setting up xbee tx rx baud rate
  xbee.begin(9600);
  /* We're ready to go! */
  Serial.println(F(""));
}

void loop(void){
  clockStatus(data);
  temperatureSensor(data);
  luxSensor(data);
  delay(250);
  turbiditySensor(data);
  printArray(data); 
  xbee.write(data);
  delay(5000);
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}
void clockStatus(char *data)
{
  char* time = rtc.getTimeStr();
  char* date = rtc.getDateStr();
  //strcpy(data, time);
  strcat(data, date);
  Serial.println("");
  Serial.print(F("TIME: ")); Serial.println(rtc.getTimeStr());
  Serial.print(F("DATE: ")); Serial.println(rtc.getDateStr());
}
void temperatureSensor(char *data)
{
    // request to all devices on the bus
  sensorsOne.requestTemperatures(); // Send the command to get temperatures
  sensorsTwo.requestTemperatures(); // Send the command to get temperatures
  Serial.println(F("------------------------------------"));
  Serial.print(F("Temperature is: "));
  static float temp1 = sensorsOne.toFahrenheit(sensorsOne.getTempCByIndex(0));
  Serial.println(temp1);
  static char tempChar1[10];
  dtostrf(temp1, 5, 2, tempChar1);
  strcat(data, tempChar1);
  Serial.print(F("Deep Temperature is: "));
  static float temp2 = sensorsTwo.toFahrenheit(sensorsTwo.getTempCByIndex(0));
  Serial.println(temp2);
  static char tempChar2[10];
  dtostrf(temp2, 5, 2, tempChar2);
  strcat(data, tempChar2);
}

void luxSensor(char *data)
{
    /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light){
  Serial.print(event.light); Serial.println(F(" lux"));
  static float lumi = event.light;
  static char lumi_char[10];
  dtostrf(lumi, 8, 2, lumi_char);
  //Serial.println(lumi_char);
  strcat(data, lumi_char);
  }else{
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
}

void turbiditySensor(char *data){
  Serial.print(F("Turbidity: "));
  int sensorValue = analogRead(turbidity); 
  Serial.println(sensorValue);
  float voltage = sensorValue * (5.0/1024.0);
  //4.20025 when the equation below is set to zero
  Serial.println(voltage);
  double NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
  Serial.println(NTU);
  static char NTU_DATA[10];
  dtostrf(NTU, 7, 2, NTU_DATA);
  strcat(data, NTU_DATA);
  delay(750);
}
void averageArray(double *array ){
   double sum, avg;
   int loop;
   sum = avg = 0;  
   for(loop = 0; loop < 10; loop++) {
      sum = sum + array[loop];
   }
   
   avg = sum / loop;
   
   Serial.println(avg);
}
void printArray(char* arr){
  for(int i = 0; i < 50; i++){
    if (arr[i] == 32){
      arr[i] = '0';
    }
    Serial.print(arr[i]);
  }
  Serial.println("");
}
