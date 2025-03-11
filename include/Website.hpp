#ifndef Website_H
#define Website_H
#include <Arduino.h>
// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSockets.h>

String processor(const String& var);

String outputState(int output);

void WebsiteSetup();

#endif 
