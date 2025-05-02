//MQTT HPP

#ifndef MQTT_HANDLER_HPP
#define MQTT_HANDLER_HPP

#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFi.h>

//MQTT CLIENT//
extern const char* ssid;
extern const char* password;

void setupWifi(); 
void reconnect(); 
void publishMIDI(String MidiMessage); 
void setUpMqtt(); 
void loopMqtt();
void callback(char* topic, byte* payload, unsigned int length);

typedef struct struct_MQTT{

    // char midiString[50];
    uint8_t Channel; 
    uint8_t Value; 
    uint8_t StatusByte; 
    uint8_t CableNumber; 
    
    } struct_MQTT;
    
  extern  struct_MQTT midiReadingMqtt; 


#endif //MQTT_HANDLER_HPP
