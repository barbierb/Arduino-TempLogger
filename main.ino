#include "DHTStable.h"
#include <SPI.h>
#include <TFT.h> 
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"


// TEMPERATURE SENSOR & VARIABLES
#define DHT11_PIN 2
DHTStable DHT;

// RTC VARIABLES
RTC_DS1307 RTC;

// DATA LOGGER BOARD SS PIN
const int chipSelect = 10;

// FILE NAME TO SAVE ALL DATA & VARIABLE
const char* filename = "data.csv";
File myFile;

// TFT LCD 1.77" SCREEN 
TFT lcd = TFT(7, 6, 5);

#define ARR_SIZE_7D 168

byte temp7d[ARR_SIZE_7D];
byte humi7d[ARR_SIZE_7D];

void setup() {
  
  Serial.begin(9600);

  initLCD();
  initSDcard();
  initRTC();
  initDatafile();
  initDHT();
  
  clearLCD();
  Serial.println(F("setup() OK"));

  DateTime bootDateTime = RTC.now();

  lcd.stroke(255,255,255);
  lcd.setTextSize(1);
  lcd.text("Now:", 20, 12);
  lcd.text("24h:", 30, 38);
  lcd.text(" 7d:", 30, 70);
  lcd.text("Since:", 13, 93);
  char _buffer_bootdate[9];
  sprintf(_buffer_bootdate, "%02d/%02d/%02d", bootDateTime.year()-2000, bootDateTime.month(), bootDateTime.day() );
  lcd.text(_buffer_bootdate, 6, 103);
  sprintf(_buffer_bootdate, "%02d:%02d", bootDateTime.hour(), bootDateTime.minute());
  lcd.text(_buffer_bootdate, 15, 113);
}

void shiftvals() {
    memmove(&temp7d[1], &temp7d[0], (ARR_SIZE_7D - 1) * sizeof(temp7d[0]));
    memmove(&humi7d[1], &humi7d[0], (ARR_SIZE_7D - 1) * sizeof(humi7d[0]));
    /*Serial.print(F("TEMP: "));
    for (unsigned int i = 0; i < (sizeof(temp7d) / sizeof(temp7d[0])); i++)
    {
        Serial.print(temp7d[i]);
        Serial.print(",");
    }
    Serial.println();
    Serial.print(F("HUMI: "));
    for (unsigned int i = 0; i < (sizeof(humi7d) / sizeof(humi7d[0])); i++)
    {
        Serial.print(humi7d[i]);
        Serial.print(",");
    }
    Serial.println();*/
}

byte minTemp, maxTemp, minHumi, maxHumi;

void loop() {
  Serial.println(F("loop() call"));

  char _buffer1[7];
  char _buffer2[7];
  char _buffer_temp_max_boot[6];
  char _buffer_temp_min_boot[6];
  char _buffer_humi_max_boot[6];
  char _buffer_humi_min_boot[6];
  
  
  int check = DHT.read11(DHT11_PIN);
  
  String data = "";
  switch (check) {
    case DHTLIB_OK:  
      data += F("OK;"); 
      break;
      
    case DHTLIB_ERROR_CHECKSUM: 
      data += F("Checksum error;"); 
      break;
      
    case DHTLIB_ERROR_TIMEOUT: 
      data += F("Time out error;"); 
      break;
      
    default: 
      data += F("Unknown error;"); 
      break;
  }
  
  DateTime now = RTC.now();

  byte currTemp = (int)DHT.getTemperature();
  byte currHumi = (int)DHT.getHumidity();

  if(currTemp > maxTemp) {
    maxTemp = currTemp;
  } else if(currTemp < minTemp) {
    minTemp = currTemp;
  }
  if(currHumi > maxHumi) {
    maxHumi = currHumi;
  } else if(currHumi < minHumi) {
    minHumi = currHumi;
  }
  
  shiftvals();

  temp7d[0] = currTemp;
  humi7d[0] = currHumi;

  byte mintemp24 = currTemp;
  byte mintemp7d = currTemp;
  byte maxtemp24 = currTemp;
  byte maxtemp7d = currTemp;
  
  byte minhumi24 = currHumi;
  byte minhumi7d = currHumi;
  byte maxhumi24 = currHumi;
  byte maxhumi7d = currHumi;

  for (int n = 0; n < (sizeof(temp7d) / sizeof(temp7d[0])); n++) {
    byte tmp = temp7d[n];
    if(mintemp7d > tmp) {
      mintemp7d = temp7d[n];
    } else if(maxtemp7d < tmp) {
      maxtemp7d = tmp;
    }

    if(n <= 24) {
      if(mintemp24 > tmp) {
        mintemp24 = temp7d[n];
      } else if(maxtemp24 < tmp) {
        maxtemp24 = tmp;
      }
    }
  }
  
  data += now.unixtime();
  data += ";";
  data += currTemp;
  data += ";";
  data += currHumi;

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

  int baseValX = 70; int baseValY = 32;

  lcd.stroke(255,255,255);
  lcd.setTextSize(2);
  
  sprintf(_buffer1, "%d\367C", currTemp);
  lcd.text(_buffer1, 50, 8);
  sprintf(_buffer2, "%d%%", currHumi);
  lcd.text(_buffer2, 110, 8);

  lcd.setTextSize(1); 
  
  lcd.stroke(252, 73, 28);
  lcd.text("01\367C", baseValX, baseValY);
  lcd.text("02\367C", baseValX, baseValY+35);
  sprintf(_buffer_temp_max_boot, "%d\367C", maxTemp);
  lcd.text(_buffer_temp_max_boot, baseValX, baseValY+70);
  lcd.text("03%", baseValX+50, baseValY);
  lcd.text("04%", baseValX+50, baseValY+35);
  sprintf(_buffer_humi_max_boot, "%d%%", maxHumi);
  lcd.text(_buffer_humi_max_boot, baseValX+50, baseValY+70);
  
  lcd.stroke(84, 241, 255);
  lcd.text("05\367C", baseValX, baseValY+12);
  lcd.text("06\367C", baseValX, baseValY+47);
  sprintf(_buffer_temp_min_boot, "%d\367C", minTemp);
  lcd.text(_buffer_temp_min_boot, baseValX, baseValY+82);
  lcd.text("07%", baseValX+50, baseValY+12);
  lcd.text("08%", baseValX+50, baseValY+47);
  sprintf(_buffer_humi_min_boot, "%d%%", minHumi);
  lcd.text(_buffer_humi_min_boot, baseValX+50, baseValY+82);

  delay(2000);

  // CLEARING OLD VALUES
  lcd.stroke(0,0,0);
  lcd.setTextSize(2);
  lcd.text(_buffer1, 50, 8);
  lcd.text(_buffer2, 110, 8);
  lcd.setTextSize(1);
  lcd.text(_buffer_temp_max_boot, baseValX, baseValY+70);
  lcd.text(_buffer_humi_max_boot, baseValX+50, baseValY+70);
  lcd.text(_buffer_temp_min_boot, baseValX, baseValY+82);
  lcd.text(_buffer_humi_min_boot, baseValX+50, baseValY+82);
  
  //delay(2000);
}

void clearLCD() {
  lcd.fill(0,0,0);
  lcd.stroke(0,0,0);
  lcd.rect(0, 0, lcd.width(), lcd.height());
}

void initDHT() {
  int check = DHT.read11(DHT11_PIN);
  
  switch (check) {
    case DHTLIB_OK:
      lcd.text("DHT temp sensor... OK", 0, 40);
      break;
      
    case DHTLIB_ERROR_CHECKSUM: 
      lcd.text("DHT temp sensor... Checksum error!", 0, 40);
      while(1);
      
    case DHTLIB_ERROR_TIMEOUT: 
      lcd.text("DHT temp sensor... Time out error!", 0, 40);
      while(1);
      
    default: 
      lcd.text("DHT temp sensor... Unknown error!", 0, 40);
      while(1);
  }

  minTemp = (int)DHT.getTemperature();
  maxTemp = (int)DHT.getTemperature();
  minHumi = (int)DHT.getHumidity();
  maxHumi = (int)DHT.getHumidity();
  
  Serial.println(F("DHT sensor... OK"));
}

void initDatafile() {

  // Check to see if the file exists:
  if (!SD.exists(filename)) {
    
    Serial.print(filename);
    Serial.println(F(" does not exists, creating it..."));
    
    myFile = SD.open(filename, FILE_WRITE);
    myFile.close();
    
    if (!SD.exists(filename)) {
      Serial.println(F("File creation failed."));
      lcd.text("CSV file creation... Failed!", 0, 30);
      while (1);
      
    } else {
      Serial.println(F("CSV file created."));
      lcd.text("CSV file created.", 0, 30);
    }
    
  } else {
      lcd.text("CSV file found.", 0, 30);
    Serial.println(F("CSV file found."));
  }
  
  myFile.print(F("REBOOTED;"));
  myFile.print(__DATE__);
  myFile.print(" ");
  myFile.print(__TIME__);
  myFile.print(";;");
  myFile.println();
  myFile.close();
  
}

void initLCD() {
  
  lcd.begin();
  lcd.background(0, 0, 0);
  lcd.stroke(255, 255, 255);
  lcd.setTextSize(1);
  lcd.text("TFT screen... OK", 0, 0);
    Serial.println(F("TFT screen... OK"));
}

void initSDcard() {
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    Serial.println(F("SD initialization failed!"));
    lcd.text("SD card... failed!", 0, 10);
    while(1);
  }
  
  Serial.println(F("SD card... OK"));
  lcd.text("SD card... OK\n", 0, 10);
}

void initRTC() {
  Wire.begin();
  RTC.begin();

  if (!RTC.isrunning()) {
    Serial.println(F("RTC is NOT running, let's set the time!"));
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  lcd.text("RTC clock... OK\n", 0, 20);
  Serial.println(F("RTC clock... OK"));
}
