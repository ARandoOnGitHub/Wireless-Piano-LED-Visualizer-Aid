//MQTT HPP

#ifndef MQTT_HANDLER_HPP
#define MQTT_HANDLER_HPP

#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFi.h>

//MQTT CLIENT//


void setupWifi(); 
void reconnect(); 
void publishMessage(); 
void setUpMqtt(); 
void loopMqtt();
void callback(char* topic, byte* payload, unsigned int length);





#endif //MQTT_HANDLER_HPP
