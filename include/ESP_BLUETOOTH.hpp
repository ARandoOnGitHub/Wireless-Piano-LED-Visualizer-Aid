#ifndef ESP_BLUETOOTH_HPP
#define ESP_BLUETOOTH_HPP

#include <Arduino.h>
#include <BLEMidi.h>

void connected();
void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp);
void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp); 
void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp); 

void bluetoothSetup(); 

void ScanNearby();


#endif