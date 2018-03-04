#include <SoftwareSerial.h>
SoftwareSerial soft_serial(10,11);
void setup(){
  // Start up our serial port, we configured our XBEE devices for 9600 bps.
  Serial.begin(9600);
  soft_serial.begin(9600);
}

void loop(){
  char data[20]="okay";
  soft_serial.write(data);
  delay(5000);
}

