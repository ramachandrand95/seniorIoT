#include <Wire.h>
#include <RTClibExtended.h>
#include <LowPower.h>

#define wakePin 2    //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW
#define ledPin 13    //use arduino on-board led for indicating sleep or wakeup status

RTC_DS3231 RTC;      //we are using the DS3231 RTC

byte AlarmFlag = 0;
byte ledStatus = 1;

//-------------------------------------------------

void wakeUp()        // here the interrupt is handled after wakeup
{
}

//------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  //Set pin D2 as INPUT for accepting the interrupt signal from DS3231
  pinMode(wakePin, INPUT);

  //switch-on the on-board led for 1 second for indicating that the sketch is ok and running
  pinMode(ledPin, OUTPUT);

  //Initialize communication with the clock
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time
  
  //clear any pending alarms
  RTC.armAlarm(1, false);
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);
  RTC.armAlarm(2, false);
  RTC.clearAlarm(2);
  RTC.alarmInterrupt(2, false);

  //Set SQW pin to OFF (in my case it was set by default to 1Hz)
  //The output of the DS3231 INT pin is connected to this pin
  //It must be connected to arduino D2 pin for wake-up
  RTC.writeSqwPinMode(DS3231_OFF);

}

//------------------------------------------------------------

void loop() {
  //On first loop we enter the sleep mode
  if (AlarmFlag == 0) {
    Serial.println("Running Samples.. LED HIGH CHECK PLEASE");
    digitalWrite(ledPin, HIGH);  
    Serial.println(F("SETTING ALARM"));
    //Set alarm1 every day at 18:33
    RTC.setAlarm(ALM1_MATCH_SECONDS, 20, 0,0,0);   //set your wake-up time here
    RTC.alarmInterrupt(1, true);
    attachInterrupt(0, wakeUp, LOW);                      //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW 
    //Setting Status of LED
    Serial.println(F("GOING TO SLEEP"));
    digitalWrite(ledPin,LOW); 
        DateTime now = RTC.now();
        //Seeing what time it is 
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
    delay(2000);//switch-off the led for indicating that we enter the sleep mode
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);   //arduino enters sleep mode here
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500);
    Serial.println(F("WOKE UP"));
    Serial.println(F("Clearing Alarms"));

     detachInterrupt(0); 
     RTC.armAlarm(1, false);
     RTC.clearAlarm(1);
     RTC.alarmInterrupt(1, false); 
     delay(500);
  }
}


