

// Copyright 2015, Matthijs Kooijman <matthijs@stdin.nl>
//
// Permission is hereby granted, free of charge, to anyone
// obtaining a copy of this document and accompanying files, to do
// whatever they want with them without any restriction, including, but
// not limited to, copying, modification and redistribution.
//
// NO WARRANTY OF ANY KIND IS PROVIDED.
//
//
// This example broadcasts a Hello, world! message every 10 seconds.

#include <XBee.h>
#include <SoftwareSerial.h>
#include <Printers.h>

#define rx 7
#define tx 8
SoftwareSerial xbee_serial(rx, tx);
#define DebugSerial Serial
#define XBeeSerial xbee_serial

XBeeWithCallbacks xbee;
String load;

void setup() {
  // Setup debug serial output
  DebugSerial.begin(115200);
  //DebugSerial.println(F("Starting..."));

  // Setup XBee serial communication
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
  delay(1);
}

int sendPacket(String packet) {
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


void loop() {
//  if(DebugSerial.available()){
//    load=DebugSerial.readString();
//    sendPacket(load);
//    //DebugSerial.print(load);
//    //DebugSerial.println(load.length());
//  }

  String t="test";
  sendPacket(t);
  delay(5000);
}
