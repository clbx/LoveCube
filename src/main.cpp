
#include <WiFi.h>
#include <Wire.h>
#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "credentials.h"

Credentials creds;

const char* ssid = creds.ssid; 
const char* password = creds.pass;
bool newmsg = false;

#define DAC1 25
#define DAC2 26
#define BUTTON 35

int left = 120; //255= 3.3V 128=1.65V
int right = 180;
bool up = true;
bool newMsg = false;

AsyncWebServer server(80);
 
void setup() {

  pinMode(BUTTON,INPUT);
  Serial.begin(9600);

  //lcd.init();
  //lcd.backlight();
 
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
 
      for (size_t i = 0; i < len; i++) {
        newMsg = true;
        Serial.write(data[i]);
        
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
