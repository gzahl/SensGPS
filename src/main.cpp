#include <Arduino.h>

#include "SoftwareSerial.h"

#include "sensesp_app.h"
#include "wiring_helpers.h"

#ifdef ESP8266
#define RXPIN D7
#elif defined(ESP32)
#define RXPIN 25
#endif

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

  SoftwareSerial* swSerial = new SoftwareSerial(RXPIN, -1);
  swSerial->begin(115200, SWSERIAL_8N1);
  
  setup_gps(swSerial);

  sensesp_app->enable();
});