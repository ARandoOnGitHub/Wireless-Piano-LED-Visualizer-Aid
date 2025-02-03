#include "ESP_BLUETOOTH.hpp"
#include "LEDHandler.hpp"
#include "ESP_NOW.hpp"
// Work On sending info to ESP_NOW Protocol

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Received note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  lightUpLED(note,velocity);
//   MidiReading.channel= note; 
//   MidiReading.value= velocity; 
//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&MidiReading, sizeof(MidiReading));
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Received note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  turnOffLED(note);

//   MidiReading.channel= note; 
//  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&MidiReading, sizeof(MidiReading));
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
    Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void connected()
{
  Serial.println("Connected");
}

void bluetoothSetup(){

BLEMidiServer.begin("MIDI device");
  BLEMidiServer.setOnConnectCallback(connected);
  BLEMidiServer.setOnDisconnectCallback([](){     // To show how to make a callback with a lambda function
    Serial.println("Disconnected");
  });
  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);
  BLEMidiServer.setControlChangeCallback(onControlChange);
  BLEMidiServer.enableDebugging();
 

}

void scanNearby(){


  
}
