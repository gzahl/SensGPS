#include <Arduino.h>

#include "HardwareSerial.h"

#include "sensesp_app.h"
#include "wiring_helpers.h"
#include <Wire.h>

#ifdef ESP8266
#define RXPIN D7
#elif defined(ESP32)
#define RXPIN RX2
#endif

TwoWire* twoWire;
HardwareSerial* hwSerial;


ReactESP app([] () {
  sensesp_app = new SensESPApp();

  // Hardware serial port is reserved for serial terminal

  Serial.begin(115200);
  Serial.available();

  // A small arbitrary delay is required to let the
  // serial port catch up
  delay(100);
  Debug.setSerialEnabled(true);

  // Software serial port is used for receiving NMEA data

  hwSerial = new HardwareSerial(2);
  hwSerial->begin(38400);

  //pinMode(LED_BUILTIN, OUTPUT);
  delay(100);

  //hwSerial->println("$PMTK220,1000*1F");
  //hwSerial->println("$PMTK251,38400*27");
  //hwSerial->flush();

  /*app.onRepeat(1, [] () {
     if(hwSerial->available()) {
       Serial.print(char(hwSerial->read()));
     }
   });*/
  
  setup_gps(hwSerial);

  sensesp_app->enable();
});