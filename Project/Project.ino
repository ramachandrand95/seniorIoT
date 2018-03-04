#include <RTClibExtended.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>   
#include <DallasTemperature.h>  
#include <XBee.h>
//#include <SoftwareSerial.h>
#include <SD.h>

//////////////FOR DEBUGGING ////////////////////////////
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
//////////////FOR DEBUGGING ////////////////////////////

//RTC
RTC_DS3231 RTC;
//Luminosity
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
//Temperature
OneWire oneWire(3);   
DallasTemperature sensors(&oneWire);
//xbee
//SoftwareSerial xbee_serial(7,8); //rx,tx
XBeeWithCallbacks xbee; 

void setup() 
{
  Serial.begin(9600);
  
  //RTC
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
  
  //Luminosity
  if(!tsl.begin())
  {
    DEBUG_PRINTLN(F("Ooops, no TSL2561 detected"));
  }
  tsl.enableAutoRange(true);           
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

  //turbidity
  pinMode(A0, INPUT);

  //Temperature
  sensors.begin();   
  sensors.setResolution(12);

  //xbee
  //xbee_serial.begin(9600);
  xbee.begin(Serial);

  //SD card
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

void loop() 
{ 
  String dataLog = RTC_Info()+'+'+getTemperatures()+'+'+getLux()+'+'+getTurbidity();
  
  DEBUG_PRINTLN();
  DEBUG_PRINT("Sending:");
  DEBUG_PRINTLN(dataLog);
  
  sendPacket(dataLog);
  writeToFile("test.txt",dataLog );
  delay(5000);
}

String RTC_Info(){
  DateTime now = RTC.now();
  return String(now.hour())+':'+String(now.minute())+':'+String(now.second())+','+String(now.month())+'/'+String(now.day())+'/'+String(now.year());
}

String getLux()
{
  float lux1=0;
  sensors_event_t event;
  for(int i = 0; i<5; i++)
  {
    tsl.getEvent(&event);
    if(event.light)
    {
      lux1 += event.light;
    }
  }
  return String(lux1/5);
}

String getTurbidity()
{
  float turbidity1=0;
  for(int i = 0; i< 10; i++){
    int sensorValue = analogRead(A0);
    //turbidity1+=analogRead(A0);
    turbidity1 += -1120.4*((sensorValue * (5.0/1024.0))*(sensorValue * (5.0/1024.0)))+5742.3*(sensorValue * (5.0/1024.0))-4352.9; 
  }
  return String(turbidity1/10);
}

String getTemperatures(){
  sensors.requestTemperatures();
  return String(sensors.toFahrenheit(sensors.getTempCByIndex(0))) + ',' + String(sensors.toFahrenheit(sensors.getTempCByIndex(1)));;
}

int sendPacket(String packet) 
{
    //extracting payload information
    //length of payload
    int payload_size=packet.length()+1;
    char payload[payload_size];
    //extracting payload to be sent in API frame
    packet.toCharArray(payload, payload_size);
    
    //creating data frame
    ZBTxRequest txRequest;
    //setting adress to 0x0 sends message to coordinator
    txRequest.setAddress64(0x0000000000000000);
    //adding payload to data frame
    txRequest.setPayload((uint8_t *)payload, payload_size);
    //send data frame 
    //check for successfull transmission.
    //returns 0 for sucessfull transmission 
    return xbee.sendAndWait(txRequest, 5000);
}

void writeToFile (String fileName,String data)
{
  File myFile = SD.open(fileName,FILE_WRITE);
  if(myFile){
    myFile.println(data);
    myFile.close();
  }
  else{
    DEBUG_PRINTLN(F("error opening file"));
  }
}
