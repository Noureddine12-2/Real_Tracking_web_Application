#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "PWD"
#endif

const char* ssid     = "khidaoui";
const char* password = "12345678900";

static const int RXPin = D1, TXPin = D2;
static const uint32_t GPSBaud = 9600;

//Replace the ngrok url here
const char* host = "http://3337-168-253-110-116.ngrok.io";
const uint16_t port = 80;

String assetnumber = "123";


TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
HTTPClient http;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ss.begin(GPSBaud);

}


void loop()
{

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

 if(client.connected() && ss.available() > 0){
    while (ss.available() > 0)
      if (gps.encode(ss.read())){
        if (gps.location.isValid()){
      
          float latitude = (gps.location.lat());
          float longitude = (gps.location.lng());
          String json;
          
          Serial.println(latitude);
      
          Serial.println(longitude);
      
          DynamicJsonDocument doc(2000);
 
          doc["assetnumber"] = assetnumber;
          doc["latitude"] = latitude;
          doc["longitude"] = longitude;

          
          serializeJson(doc, json);
          Serial.println(json);
          //Replace the ngrok url here
          http.begin(client,"http://3337-168-253-110-116.ngrok.io/api/pin");        //Specify request destination
          http.addHeader("Content-Type", "application/json");
          int httpCode = http.POST(json);   //Send the request
          String payload = http.getString();                                        //Get the response payload
 
          Serial.println(httpCode);   //Print HTTP return code
          Serial.println(payload);    //Print request response payload
 
          http.end(); 
          delay(10000);
        }
      
        else{
          Serial.println("GPS data not valid");
        }
      }
  }
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  
}
