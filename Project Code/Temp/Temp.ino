#include <XBee.h>
#include <SoftwareSerial.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <RTClibExtended.h>

#include <SD.h>
//#include <LowPower.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

//#include <SPI.h>

#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.print(x)
 #define DEBUG_WRITE(x,y)  Serial.write (x,y)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_WRITE(x,y)
 #define DEBUG_PRINT(x)
#endif

// Data wire is plugged into pin 2 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(3);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//File file;
RTC_DS3231 RTC;
//setting up xbee transmit and receive pins.

SoftwareSerial xbee_serial(7, 8);
XBeeWithCallbacks xbee;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
 /*********************************************/

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  //Initialize communication with the clock
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time
  pinMode(A0, INPUT);

  // Start up temperature sensor library
  sensors.begin();  
  sensors.setResolution(12);
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    DEBUG_PRINTLN(F("Ooops, no TSL2561 detected"));
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  /* Setup the sensor gain and integration time */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  //setting up xbee tx rx baud rate
  xbee_serial.begin(9600);
  xbee.begin(xbee_serial);

  //SD card init
  initializeSD();
  }

void loop(void){
  String dataLog = RTC_Info()+','+getTemperatures()+','+getLux()+','+getTurbidity();
 // Serial.println(dataLog);
  sendPacket(dataLog);
  writeToFile("test.txt",dataLog);
}
String RTC_Info(){
  DateTime now = RTC.now();
  return String(now.hour())+':'+String(now.minute())+':'+String(now.second())+','+String(now.month())+'/'+String(now.day())+'/'+String(now.year());
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
  float lux1=0;
  for(int i = 0; i<10; i++){
     /* Get a new sensor event */ 
    sensors_event_t event;
    tsl.getEvent(&event);
    if(event.light){
      lux1 += event.light;
    }
    else{
      DEBUG_PRINTLN(F("Sensor overload"));
    }
  }
  return String(lux1/10);
}

String getTurbidity(){
 float turbidity1=0;
  for(int i = 0; i< 10; i++){
     //Serial.println("turbidity sensor value: " + sensorValue);
     //float voltage = sensorValue * (5.0/1024.0);
     //float NTU = -1120.4*(voltage*voltage)+5742.3*(voltage)-4352.9;
     //turbidity1[i] = NTU;
    int sensorValue = analogRead(A0);
    //turbidity1+=analogRead(A0);
    turbidity1 += -1120.4*((sensorValue * (5.0/1024.0))*(sensorValue * (5.0/1024.0)))+5742.3*(sensorValue * (5.0/1024.0))-4352.9; 
  }
  return String(turbidity1/10);
}

int sendPacket(String packet) {
    //extracting payload from data packet
    int payload_size=packet.length()+1;
    char payload[payload_size];
    packet.toCharArray(payload, payload_size);
    
    DEBUG_PRINTLN(F("SENDING: "));
    DEBUG_WRITE((uint8_t *)payload, payload_size);
    
    ZBTxRequest txRequest;
    txRequest.setAddress64(0x0000000000000000);
    txRequest.setPayload((uint8_t *)payload, payload_size);

    uint8_t status = xbee.sendAndWait(txRequest, 5000);
    if (status == 0) {
      DEBUG_PRINTLN(F("Succesfully sent packet"));
      return 0;
    } else {
      DEBUG_PRINTLN(F("Failed to send packet. Status: 0x"));
      return 1;
    }
}



//initialize SD
void initializeSD()
{
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);
  if (SD.begin())
  {
    DEBUG_PRINTLN(F("SD card is ready to use."));
  } else
  {
    DEBUG_PRINTLN(F("SD card initialization failed"));
  }
}



//fuction: write to data to SD card
//parameter: fileName, data
//return: void 
void writeToFile (String fileName,String data){
  File myFile = SD.open(fileName,FILE_WRITE);
  if(myFile){
    myFile.println(data);
    myFile.close();
  }
  else{
    DEBUG_PRINTLN(F("error opening file"));
  }
}

