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

String scanNearbyDevices();
String processor(const String& var);

String outputState(int output);

void WebsiteSetup();
void setupAP();


#endif 
