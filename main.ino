#include "DHTStable.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <WiFiClient.h>
#include "secrets.h"

const char *host = "script.google.com";
const int httpsPort = 443;  // HTTPS

// SHA-1 fingerprint of script.google.com 's certificate
// determined using a web browser
// new fingerprint from script.google.com 's certificate, cannot be automatically handled.
const char fingerprint[] = "B4 B4 14 59 B2 62 3D 11 1E 57 51 E0 86 6F 98 1C 98 91 C2 B1";

// TEMPERATURE SENSOR & VARIABLES
#define DHT22_PIN D2
DHTStable DHT;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}



void loop() {

  if(WiFi.status()== WL_CONNECTED) {

    int check = DHT.read22(DHT22_PIN);
    
    if(check != DHTLIB_OK) {
      Serial.println(F("DHT temp sensor not working..."));
      delay(10000);
      return;
    }

    float currTemp = DHT.getTemperature();
    float currHumi = DHT.getHumidity() / 100.0;
    
    WiFiClientSecure httpsClient;
    httpsClient.setTimeout(5000);

    Serial.print(F("connecting to "));
    Serial.println(host);
  
    //Serial.printf("Using fingerprint '%s'\n", fingerprint);
    //httpsClient.setFingerprint(fingerprint);
    httpsClient.setInsecure();
    
    
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 3)){
        delay(1000);
        Serial.print(F("."));
        r++;
    }
    if(r==3) {
      Serial.println(F("Connection failed"));
      return;
      
    } else {
      Serial.print(F("Connected to "));
      Serial.println(host);
    }

    String path = baseurl + "temperature=" + currTemp + "&humidity=" + currHumi;
    path.replace(".", ",");

    httpsClient.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +  
               "User-Agent: ESP8266-Client\r\n" +             
               "Connection: close\r\n\r\n");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
  
    Serial.println(F("=========="));
    String line;
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    
  } else {
    
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, pass);

    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
  
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  }
  
  delay(60*60*1000); // waits 60 mins
}
