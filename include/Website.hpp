#ifndef Website_H
#define Website_H

#include <Arduino.h>
// Import required libraries
#include <WiFiManager.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSockets.h>
#include <ESPmDNS.h>
#include <FastLED.h>

#define ONBOARD_LED_PIN 48
extern CRGB onboardLed[1];

String scanNearbyDevices();
String processor(const String& var);

String outputState(int output);

extern bool sendMidiUpdate;

void WebsiteSetup();
void setupAP();
void setupOnboardLED();

#endif 
