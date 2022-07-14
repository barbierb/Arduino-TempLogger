#include "DHTStable.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// TEMPERATURE SENSOR & VARIABLES
#define DHT11_PIN       2
DHTStable DHT;

// RTC VARIABLES
RTC_DS1307 RTC;

// DATA LOGGER BOARD SS PIN
const int chipSelect = 10;

// FILE NAME TO SAVE ALL DATA & VARIABLE
const char* filename = "data.csv";
File myFile;

void setup() {
  
  Serial.begin(9600);

  initSDcard();
  initRTC();

  // Check to see if the file exists:
  if (!SD.exists(filename)) {
    
    Serial.print(filename);
    Serial.println(F(" does not exists, creating it..."));
    
    myFile = SD.open(filename, FILE_WRITE);
    myFile.close();
    
    if (!SD.exists(filename)) {
      Serial.println(F("File creation failed."));
      while (1);
    }
    
  }
  
  myFile.print("REBOOTED;");
  myFile.print(__DATE__);
  myFile.print(" ");
  myFile.print(__TIME__);
  myFile.print(";;");
  myFile.println();
  myFile.close();
}


void loop() {
  
  String data = "";
  
  int check = DHT.read11(DHT11_PIN);
  
  switch (check) {
    case DHTLIB_OK:  
      data += "OK;"; 
      break;
      
    case DHTLIB_ERROR_CHECKSUM: 
      data += "Checksum error;"; 
      break;
      
    case DHTLIB_ERROR_TIMEOUT: 
      data += "Time out error;"; 
      break;
      
    default: 
      data += "Unknown error;"; 
      break;
  }
    
  DateTime now = RTC.now();
  
  data += now.timestamp();
  data += ";";
  data += DHT.getHumidity();
  data += ";";
  data += DHT.getTemperature();

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    Serial.println(data);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(filename);
  }

  delay(60000);
  
}

void initSDcard() {
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }
  
  Serial.println(F("card initialized."));
}

void initRTC() {
  Wire.begin();
  RTC.begin();

  if (!RTC.isrunning()) {
    
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
