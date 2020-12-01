/*
   ESP-12-02 V1_A1
   Copyright 2020-2020 Krzysztof Karolak

   ESP12-based ILife A4S Cleaning Robot Controller

   OTA Settings - 115200 4M(1M SPIFFS)

   ESP-12 PHYSICAL PINOUT

   GP04 - IR LED

   VIRTUAL PINOUT
   V10 - JOYSTICK
   V11 - CLEAN
   V12 - SPIRAL CLEANING
   V13 - EDGE CLEANING
   V14 - BACK TO BASE
   V20 - AUTO CLEANING TIME
   V21 - AUTO CLEANING
   
   EXT - External Data (received from server)
   V67 - OWNER HOME

*/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include "arduino_secrets.h" // Load passwords and API Keys

char auth[] = SECRET_APIKEY; // Blynk Token

/* Blynk Widgets Init */
WidgetRTC rtc;

#define IR_LED 4
IRsend irsend(IR_LED);

/* Variables assignment */
boolean isOwnerHome = false;


/* --------------- IR Remote Functions ---------------*/
void MoveIR(int moveDirection = 0) {
  switch(moveDirection) {
    case 0: //UP
      irsend.sendNEC(0x2AA55AA, 32);
      break;
    case 1: //DOWN
      irsend.sendNEC(0x2AA6699, 32);
      break;
    case 2: //LEFT
      irsend.sendNEC(0x2AA33CC, 32);
      break;
    case 3: //RIGHT
      irsend.sendNEC(0x2AA44BB, 32);
      break;
  }
}

BLYNK_WRITE(V10) { //JOYSTICK
  int moveX = param[0].asInt();
  int moveY = param[1].asInt();

  if(moveX < 100) {
    MoveIR(2); // Move Left
  } else if(moveX > 900) {
    MoveIR(3); //Move Right
  }

  if(moveY > 900) {
    MoveIR(0); //Move Up
  } else if(moveY < 100) {
    MoveIR(1); //Move Down
  }
}

BLYNK_WRITE(V11) { //CLEAN Button
  if(param.asInt()) {
    irsend.sendNEC(0x2AA22DD, 32);
  }
}

BLYNK_WRITE(V14) { //BACK TO BASE Button
  if(param.asInt()) {
    irsend.sendNEC(0x2AA8877, 32);
  }
}

BLYNK_WRITE(V67) {
  if (param.asInt()) {
    isOwnerHome = true;
  }
  else {
    isOwnerHome = false;
  }
}

void setup() {
    /* ----- Initialization ---- */
  Blynk.begin(auth, SECRET_SSID, SECRET_PASS, SECRET_SERVER_URL);
  int myTimeout = millis() / 1000;
  while (Blynk.connect() == false) { // try to connect to server for 10 seconds
    if ((millis() / 1000) > myTimeout + 8) { // try local server if not connected within 9 seconds
      break;
    }
  }
  irsend.begin();

    /* ----------- OTA ------------- */
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.onStart([]() {
  });
  ArduinoOTA.begin();

  setSyncInterval(60000); //RTC Sync
}

BLYNK_CONNECTED() {
  
}

void loop() {
  Blynk.run();
  yield();
  ArduinoOTA.handle();
}
