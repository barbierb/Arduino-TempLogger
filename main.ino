#include "DHTStable.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define DHT11_PIN       2
tmElements_t tm;
const int chipSelect = 10;
const char* filename = "data.csv";
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
DHTStable DHT;
Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;

void setup() {
  
  Serial.begin(9600);

  initSDcard();
  initRTC();

myFile = SD.open("example.txt", FILE_WRITE);
  myFile.close();

  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }
  

  myFile = SD.open("data.txt", FILE_WRITE);
  myFile.close();

  if(SD.exists("data.txt")) {
    Serial.println("exists");
  }

  return;

  /*File dataFile;
  Serial.println("\nO_CREAT routine: ");
  if (!SD.exists(filename)) {
    Serial.println("O_CREAT:");
    dataFile = SD.open(filename, O_CREAT);
  } 
  Serial.println("\nChecking to see if O_CREAT worked: ");
  if (dataFile) {
    Serial.println("Output file created using O_CREAT");
  } else {
    Serial.println("Error: O_CREAT did not work");
  }
  
  if (dataFile) {
    dataFile.println("REBOOTED;");
    dataFile.println(__TIME__);
    dataFile.println(";;");
    dataFile.close();
    
  } else {
    Serial.print("START: error opening ");
    Serial.println(filename);
  }*/
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

  data += __TIME__;
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

  delay(2000);
  
}

void initSDcard() {
  pinMode(10, OUTPUT);
  
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
    
  } else {
    Serial.println("Wiring is correct and a card is present.");
    
  }

    // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  
  Serial.println("card initialized.");
}

void initRTC() {
  
  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
