

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

SoftwareSerial xbee_serial(10, 11);
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

void sendPacket(String packet) {
    //extracting payload from data packet
    int payload_size=packet.length()+1;
    char payload[payload_size];
    packet.toCharArray(payload, payload_size);
    
    DebugSerial.print("SENDING: ");
    DebugSerial.write((uint8_t *)payload, payload_size);
    DebugSerial.println(" ");
    
    ZBTxRequest txRequest;
    txRequest.setAddress64(0x0000000000000000);
    txRequest.setPayload((uint8_t *)payload, payload_size);
    
    //uint8_t payload[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    //char payload[] = "hello";
    //txRequest.setPayload((uint8_t *)payload, sizeof(payload));
    
    uint8_t status = xbee.sendAndWait(txRequest, 5000);
    if (status == 0) {
      DebugSerial.println(F("Succesfully sent packet"));
    } else {
      DebugSerial.print(F("Failed to send packet. Status: 0x"));
      DebugSerial.println(status, HEX);
    }
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
