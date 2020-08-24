#include <WiFi.h>
#include <Wire.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <SSD_13XX.h>
#include "credentials.h"
#include "_fonts/Terminal_9.c"

Credentials creds;

const char* ssid = creds.ssid; 
const char* password = creds.pass;
const char* server = creds.server;
int id = creds.id;
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
String storedMessage = "NOTWHATITLLBE";
HTTPClient http;

String serverpath = String(server + String("/api/getMessage?token=" + String(id)));

int delayTimer = 0;
 
void setup() {

  pinMode(BUTTON,INPUT);
  Serial.begin(9600);

  long unsigned debug_start = millis();
  while (!Serial && ((millis() - debug_start) <= 5000));
  tft.begin(false);
  delay(30);
  errorCode = tft.getErrorCode();
  if (errorCode != 0) {
	  Serial.print("Init error! ");
	  if (bitRead(errorCode, 0)) Serial.print("MOSI or SCLK pin mismach!\n");
	  if (bitRead(errorCode, 1)) Serial.print("CS or DC pin mismach!\n");
  }
  else {
	  Serial.println(F("LoveCube Started"));
    tft.setFont(&Terminal_9);
	  tft.setTextColor(WHITE);
	  tft.setTextScale(2);
    tft.setRotation(3);
  }
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.println("Connecting\n to WiFi..");
  }
  
  tft.clearScreen();
  tft.println("Connected");
  tft.print("IP: ");
  tft.print(WiFi.localIP());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  delay(5000);
  tft.clearScreen();

}

void loop() {
  //Every 10 seconds check for a new message
  if(delayTimer == 100){
    Serial.println("Checking for Message");
    http.begin(serverpath.c_str());
    int httpResponseCode = http.GET();

    if(httpResponseCode >0){
      Serial.print("HTTP Response: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.print("Got message: ");
      Serial.println(payload);
      if(payload != storedMessage){
        Serial.println("New message :)");
        newMsg = true;
        storedMessage = payload;
      }
      else{
        Serial.println("Not new message :(");
      }
      
    }

    http.end();
    delayTimer = 0;
  }else{
    delayTimer++;
  }
  
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
