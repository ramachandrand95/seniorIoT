
#include <SoftwareSerial.h>

char myData;
int const ledpin = 13;

SoftwareSerial soft_serial(10,11);
void setup(){
  // Start up our serial port, we configured our XBEE devices for 9600 bps.
  Serial.begin(9600);
  soft_serial.begin(9600);
  //Serial1.begin(9600);
  pinMode(ledpin, OUTPUT);
}

void loop(){
  char data[20]="okay";
  if(Serial.available() > 0){
   
      myData = Serial.read();
      //Serial1.print(myData);
      //Serial1.print(data);
      //Serial1.write(myData);
      soft_serial.write(myData);
      /*
      if(myData == '1')
      {
      digitalWrite(ledpin, HIGH);
      //Serial.print(myData,HEX);
      Serial.print("\n Hi");
      //XBee.write(myData);
      }
     
      if(myData == '2')
      {
      digitalWrite(ledpin, LOW);
      //Serial.print(myData,HEX);
      Serial.print("\n low");
      }
      */
}
}

