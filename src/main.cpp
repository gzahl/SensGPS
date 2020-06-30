#include <Arduino.h>

#include "HardwareSerial.h"
#include <SoftwareSerial.h>

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
SoftwareSerial* swSerial;
SoftwareSerial* swSerialAIS;

char buffer[100];
int i = 0;

inline int calculateChecksum (const char *msg)
{
    int checksum = 0;
    for (int i = 0; msg[i] && i < 32; i++)
        checksum ^= (unsigned char)msg[i];

    return checksum;
}

inline int nemaMsgSend (const char *msg)
{
    char checksum[8];
    snprintf(checksum, sizeof(checksum)-1, "*%.2X", calculateChecksum(msg));
    hwSerial->print("$");
    hwSerial->print(msg);
    hwSerial->println(checksum);
}

inline int nemaMsgDisable (const char *nema)
{
    if (strlen(nema) != 3) return 0;

    char tmp[32];
    snprintf(tmp, sizeof(tmp)-1, "PUBX,40,%s,0,0,0,0", nema);
    //snprintf(tmp, sizeof(tmp)-1, F("PUBX,40,%s,0,0,0,0,0,0"), nema);
    nemaMsgSend(tmp);

    return 1;
}

inline int nemaMsgEnable (const char *nema)
{
    if (strlen(nema) != 3) return 0;

    char tmp[32];
    snprintf(tmp, sizeof(tmp)-1, "PUBX,40,%s,0,1,0,0", nema);
    //snprintf(tmp, sizeof(tmp)-1, F("PUBX,40,%s,0,1,0,0,0,0"), nema);
    nemaMsgSend(tmp);
    return 1;
}

ReactESP app([] () {
  //sensesp_app = new SensESPApp();

  // Hardware serial port is reserved for serial terminal

  Serial.begin(115200);
  Serial.available();

  // A small arbitrary delay is required to let the
  // serial port catch up
  delay(100);
  Debug.setSerialEnabled(true);

  // Software serial port is used for receiving NMEA data

  hwSerial = new HardwareSerial(2);
  hwSerial->begin(9600);

  swSerial = new SoftwareSerial(GPIO_NUM_18, GPIO_NUM_5, false);
  swSerial->begin(4800);
/*
  swSerialAIS = new SoftwareSerial(GPIO_NUM_19, '\377', false);
  swSerialAIS->begin(38400);
*/
  //pinMode(LED_BUILTIN, OUTPUT);
  delay(250);

  // Baudrate 38400 1Hz
  //hwSerial->println("$PMTK220,1000*1F");
  //hwSerial->println("$PMTK251,38400*27");
  //hwSerial->println("$PUBX,41,1,0003,0001,38400,0*26");
  //hwSerial->println("$PUBX,41,1,0007,0003,38400,0*18");


  //Baudrate 4800 1Hz
  //hwSerial->println("$PMTK220,1000*1F");
  //hwSerial->println("$PMTK251,4800*14");
  hwSerial->print("$PUBX,41,1,0007,0003,4800,0*13\r\n");
    
  hwSerial->flush();
  hwSerial->end();
  delay(200);
  hwSerial->end();
  hwSerial->begin(4800);
  delay(200);
  //hwSerial->println("$PMTK314,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1*28");
  hwSerial->flush();

  nemaMsgDisable("GSV");
  nemaMsgDisable("VTG");
  nemaMsgDisable("GSA");


  nemaMsgEnable("GLL");
  nemaMsgEnable("GGA");
  nemaMsgEnable("RMC");
  nemaMsgEnable("GNS");



/*
  app.onRepeat(1, [] () {
     if(hwSerial->available()) {
       byte singleByte = hwSerial->read();
       Serial.print(char(singleByte));
       //swSerial->print(singleByte);
       hwSerial->print(singleByte);
       //hwSerial->print(char(singleByte));
     }
   });*/

  
  app.onAvailable(*hwSerial, [](){
    int byte = hwSerial->read();
    if(char(byte) == '$') {
      //if(i>12 && char(buffer[5])=='L') {
      if(i>12) {
        //swSerial->write(buffer, i);
        for (int j = 0; j<i; j++){
          //swSerial->print(char(buffer[j]));
          swSerial->print(char(buffer[j]));
          Serial.print(char(buffer[j]));
        }
        //hwSerial->flush();
        //swSerial->flush();
      }
      i = 0;
    }
    buffer[i] = byte;
    i++;
  });

  /*
  app.onAvailable(*swSerial, [](){
    int byte = swSerial->read();
    //Serial.print(char(byte));
  });

  app.onAvailable(*swSerialAIS, [](){
    int byte = swSerialAIS->read();
    //Serial.print(char(byte));
  });
  */
  //setup_gps(hwSerial);

  //sensesp_app->enable();
});

