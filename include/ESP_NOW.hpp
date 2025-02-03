#ifndef ESP_NOW_HPP
#define ESP_NOW_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>


extern uint8_t broadcastAddress[];

typedef  struct struct_message {
    uint8_t channel;
    uint8_t value;
    uint8_t statusByte;
    uint8_t cableNumber;
    
    
    
} struct_message;

// Declare a global instance to be shared
extern struct_message MidiReading;

void readMacAddress(); 
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status); 
void onDataReceive(const uint8_t *mac_addr, const uint8_t *data, int len);
void sendMidiData();
void initESP_NOW(); 

#endif //ESP_NOW_HPP
