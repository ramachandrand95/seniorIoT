#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <OneWire.h> 
#include <LowPower.h>
#include <DallasTemperature.h>
#include <DS3231.h>
#include <LiquidCrystal.h> 

// Data wire is plugged into pin 2 on the Arduino
#define TEMP_ONE 3
#define TEMP_TWO 9
#define photoCell A0
#define turbidity A1 
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWireOne(TEMP_ONE);
OneWire oneWireTwo(TEMP_TWO);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensorsOne(&oneWireOne);
DallasTemperature sensorsTwo(&oneWireTwo);


DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 

int buttonPin = 9;
int flag = 0;
int buttonState = 0;

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   I2C Address
   ===========
   The address will be different depending on whether you leave
   the ADDR pin floating (addr 0x39), or tie it to ground or vcc. 
   The default addess is 0x39, which assumes the ADDR pin is floating
   (not connected to anything).  If you set the ADDR pin high
   or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
   (0x29) respectively.
    
   History
   =======
   2013/JAN/31  - First version (KTOWN)
*/
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void sampleTemps(){
  double sensor1[10] ={};
  double sensor2[10] ={};
  for(int i = 0; i<10; i++){
    sensor1[i] = sensorsOne.toFahrenheit(sensorsOne.getTempCByIndex(0));
    sensor2[i] = sensorsTwo.toFahrenheit(sensorsTwo.getTempCByIndex(0));
  }
  averageArray(sensor1);
  averageArray(sensor2);
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



/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
   tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("402 ms");
  Serial.println("------------------------------------");
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
 
void setup(void)
{
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  rtc.begin(); // Initialize the rtc object
  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
  
  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(12, 02, 55);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(14, 1, 2018);
   
  pinMode(photoCell, INPUT);
  pinMode(turbidity, INPUT);
  pinMode(buttonPin, INPUT);

  // start serial port
  Serial.begin(9600);

  // Start up temperature sensor library
  sensorsOne.begin();
  sensorsTwo.begin();
  
   Serial.println("Light Sensor Test"); Serial.println("");
   /* Initialise the sensor */
  //use tsl.begin() to default to Wire, 
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  Serial.println("");
}

void printArray(char* arr){
  for(int i = 0; i < 40; i++){
    if (arr[i] == 32){
      arr[i] = '0';
    }
    Serial.print(arr[i]);
  }
  
  
}
 
 
void loop(void)
{
  /*******************************************************/
  char* time = rtc.getTimeStr();
  char* date = rtc.getDateStr();
  char* data;
  data = malloc(strlen(time)+40);
  strcpy(data, time);
  strcat(data, date);
  Serial.println("");
  Serial.print("TIME: "); Serial.println(rtc.getTimeStr());
  Serial.print("DATE: "); Serial.println(rtc.getDateStr());
  /*******************************************************/

  
  // request to all devices on the bus
  sensorsOne.requestTemperatures(); // Send the command to get temperatures
  sensorsTwo.requestTemperatures(); // Send the command to get temperatures
  Serial.println("------------------------------------");
  Serial.print("Temperature is: ");
  static float temp1 = sensorsOne.toFahrenheit(sensorsOne.getTempCByIndex(0));
  Serial.println(temp1);
  static char tempChar1[10];
  dtostrf(temp1, 5, 2, tempChar1);
  strcat(data, tempChar1);
  Serial.print("Deep Temperature is: ");
  static float temp2 = sensorsTwo.toFahrenheit(sensorsTwo.getTempCByIndex(0));
  Serial.println(temp2);
  static char tempChar2[10];
  dtostrf(temp2, 5, 2, tempChar2);
  strcat(data, tempChar2);

  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
  Serial.print(event.light); Serial.println(" lux");
  static float lumi = event.light;
  static char lumi_char[10];
  dtostrf(lumi, 8, 2, lumi_char);
  //Serial.println(lumi_char);
  strcat(data, lumi_char);
  printArray(data);
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  delay(100000000);
  // You can have more than one IC on the same bus. 
  // 0 refers to the first IC on the wire
  //Serial.print("PhotoCell: ");
  //Serial.println(map(analogRead(photoCell),0,1023,0,100));
  
  Serial.print("Turbidity: ");
  int sensorValue = analogRead(turbidity); 
  Serial.println(sensorValue);
  float voltage = sensorValue * (5.0/1024.0);
  //(-1120.4(2.5)^2)+5742.3(2.5)-4352.9
  Serial.println(voltage);
  delay(500);
  
  delay(250);
  Serial.println("------------------------------------");
  Serial.println("");
}

