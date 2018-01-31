//#include <Printers.h>
#include <XBee.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <RTClibExtended.h>
#include <OneWire.h> 
//#include <LowPower.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

// Data wire is plugged into pin 2 on the Arduino

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(3);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//File file;
RTC_DS3231 RTC;
//setting up xbee transmit and receive pins.////////////////////////////////////////
SoftwareSerial xbee_serial(7, 8);
XBeeWithCallbacks xbee;
///////////////////////////////////////////////////////////////////////////////////
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
 /*********************************************/

void setup(void)
{
  //Initialize communication with the clock
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time
  pinMode(A0, INPUT);

  // start serial port
  Serial.begin(9600);

  // Start up temperature sensor library
  sensors.begin();  
  sensors.setResolution(12);
  /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print(F("Ooops, no TSL2561 detected"));
    while(1);
  }
  /* Display some basic information on this sensor */
  sensor_t sensor;
  tsl.getSensor(&sensor);
  
  /* Setup the sensor gain and integration time */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  //setting up xbee tx rx baud rate//////////////////////////////
  xbee_serial.begin(9600);
  xbee.begin(xbee_serial);
  }

void loop(void){
  String dataLog = RTC_Info()+','+getTemperatures()+','+getLux()+','+getTurbidity();
 // Serial.println(dataLog);
  sendPacket(dataLog);
//  writeToFile(dataLog);
}
String RTC_Info(){
  DateTime now = RTC.now();
  String data = String(now.hour())+':'+String(now.minute())+':'+String(now.second())+','+
  String(now.month())+'/'+String(now.day())+'/'+String(now.year());
  return data;
}

String getTemperatures(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  //float temp1 = sensors.toFahrenheit(sensors.getTempCByIndex(0));
  //float temp2 = sensors.toFahrenheit(sensors.getTempCByIndex(1));
  //String temp = String(temp1) + ',' + String(temp2);
  //or
  //String temp = String(sensors.toFahrenheit(sensors.getTempCByIndex(0))) + ',' + String(sensors.toFahrenheit(sensors.getTempCByIndex(1)));
  return String(sensors.toFahrenheit(sensors.getTempCByIndex(0))) + ',' + String(sensors.toFahrenheit(sensors.getTempCByIndex(1)));;
}

String getLux()
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
      Serial.println(F("Sensor overload"));
    }
  }
  String average = String(averageArray(lux1));
  //Serial.println(average);
  return average;
}

String getTurbidity(){
 float turbidity1[10];
  for(int i = 0; i< 10; i++){
     int sensorValue = analogRead(A0);
     //Serial.println("turbidity sensor value: " + sensorValue);
     float voltage = sensorValue * (5.0/1024.0);
     float NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
     //Serial.println("Turbidity: " + NTU);
     turbidity1[i] = NTU;
  }
  String average = String(averageArray(turbidity1));
  //Serial.print(average);Serial.println(" NTU");
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
    
    Serial.print(F("SENDING: "));
    Serial.write((uint8_t *)payload, payload_size);
    Serial.println(' ');
    
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

