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
SoftwareSerial xbee_serial(8, 9);
XBeeWithCallbacks xbee;
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
  String Date = getDate();
  String TimeStatus = getTimeStatus();
  String Temp1 = String(getTempOne());
  String Temp2 = String(getTempTwo());
  delay(250);
  String Lux = String(getLux());
  String waterClarity = String(getTurbidity());
  String dataLog = Date+","+TimeStatus+","+Temp1+","+Temp2+","+Lux+","+waterClarity;
  xbee.write(data);
  delay(5000);
}
String getTimeStatus()
{
  Serial.print(F("TIME: ")); Serial.println(rtc.getTimeStr());
  return rtc.getTimeStr();
}

String getDate()
{
  Serial.print(F("DATE: ")); Serial.println(rtc.getDateStr());
  return rtc.getDateStr();
}
float getTempOne()
{
  sensorsOne.requestTemperatures(); // Send the command to get temperatures
  Serial.print(F("Temperature is: "));
  static float temp1 = sensorsOne.toFahrenheit(sensorsOne.getTempCByIndex(0));
  Serial.println(temp1);
  return temp1;
}

float getTempTwo()
{
  sensorsTwo.requestTemperatures(); // Send the command to get temperatures
  Serial.print(F("Deep Temperature is: "));
  static float temp2 = sensorsTwo.toFahrenheit(sensorsTwo.getTempCByIndex(0));
  Serial.print(temp2);
  return temp2;
}

float getLux()
{
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

double getTurbidity(){
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
int sendPacket(String packet) {
    //extracting payload from data packet
    int payload_size=packet.length()+1;
    char payload[payload_size];
    packet.toCharArray(payload, payload_size);
    
    Serial.print("SENDING: ");
    Serial.write((uint8_t *)payload, payload_size);
    Serial.println(" ");
    
    ZBTxRequest txRequest;
    txRequest.setAddress64(0x0000000000000000);
    txRequest.setPayload((uint8_t *)payload, payload_size);
    
    uint8_t status = xbee.sendAndWait(txRequest, 5000);
    if (status == 0) {
      Serial.println(F("Succesfully sent packet"));
      return 0;
    } else {
      Serial.print(F("Failed to send packet. Status: 0x"));
      Serial.println(status, HEX);
      return 1;
    }
}
