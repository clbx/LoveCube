#include <WiFi.h>
#include <Wire.h>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <SSD_13XX.h>
#include "credentials.h"

Credentials creds;

const char* ssid = creds.ssid; 
const char* password = creds.pass;
bool newmsg = false;



#define DAC1 25
#define DAC2 26
#define BUTTON 35

#define _cs   17
#define _dc   16
#define _mosi 23
#define _sclk 18
#define _rst  5

SSD_13XX tft = SSD_13XX(_cs, _dc);
uint8_t errorCode = 0;

int left = 120; //255= 3.3V 128=1.65V
int right = 180;
bool up = true;
bool newMsg = false;

AsyncWebServer server(80);
 
void setup() {

  pinMode(BUTTON,INPUT);
  Serial.begin(9600);

  long unsigned debug_start = millis();
  while (!Serial && ((millis() - debug_start) <= 5000));
  tft.begin(false);
  delay(30);
  //the following it's mainly for Teensy
  //it will help you to understand if you have choosed the
  //wrong combination of pins!
  errorCode = tft.getErrorCode();
  if (errorCode != 0) {
	  Serial.print("Init error! ");
	  if (bitRead(errorCode, 0)) Serial.print("MOSI or SCLK pin mismach!\n");
	  if (bitRead(errorCode, 1)) Serial.print("CS or DC pin mismach!\n");
  }
  else {
	  Serial.println(F("Benchmark Sketch V1.1"));
	  tft.setTextColor(WHITE);
	  tft.setTextScale(2);
  }
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
 
  server.on(
    "/post",
    HTTP_POST,
    [](AsyncWebServerRequest * request){},
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      tft.clearScreen();
      for (size_t i = 0; i < len; i++) {
        newMsg = true;
        Serial.print((char)data[i]);
        tft.print((char)data[i]);
      }
      Serial.println();
      request->send(200);
  });
  server.begin();
}
 
void loop() {
  
  if(up){
    left++;
    right--;
  }
  else{
    left--;
    right++;
  }
  
  if(digitalRead(BUTTON) == HIGH){
    Serial.print("Message Read");
    newMsg = false;
  }

  if(left == 180 || left == 120){
    up = !up;
  }
  
  if(newMsg){
    dacWrite(DAC1, left);
    dacWrite(DAC2, right);
  }
  else{
    dacWrite(DAC1, 0);
    dacWrite(DAC2,0);
  }
  delay(100);
  
}
