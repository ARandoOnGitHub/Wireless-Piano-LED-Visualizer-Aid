#include "MIDIHandler.hpp"
#include "LEDHandler.hpp"
#include "ESP_NOW.hpp"
static void sendESP32Log(const String& message) {
  Serial.print(message); // Print message to Serial
}



void processMIDI(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i += 4) {
    if ((data[i] + data[i + 1] + data[i + 2] + data[i + 3]) == 0) break;
    
    // Parse MIDI data (From OG Code)
    uint8_t cableNumber = data[i] & 0x0F;
    uint8_t statusByte = data[i + 1];
    uint8_t channel = data[i + 2];
    uint8_t value = data[i + 3];

//ESP_NOW Acquiring Data

 

     // Format the parsed MIDI data as a string
        char midiString[50];  // Adjust the size as needed
        snprintf(midiString, sizeof(midiString), "Ch%d %s Channel: %d Value: %d",
                 cableNumber, (statusByte >= 0x80 && statusByte < 0x90) ? "Note Off" : (statusByte >= 0x90 && statusByte < 0xA0) ? "Note On"
                 : (statusByte >= 0xB0 && statusByte < 0xC0) ? "Control Change"
                 : "Other",
                 channel, value);

        // Raw data
        Serial.println(midiString);

        MidiReading.cableNumber = cableNumber;
        MidiReading.statusByte = statusByte;
        MidiReading.channel = channel;
        MidiReading.value = value;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&MidiReading, sizeof(MidiReading));
    // // Print MIDI message to serial monitor
    // Serial.print("MIDI Message: ");
    // Serial.print(statusByte, HEX);
    // Serial.print(" ");
    // Serial.print(channel, HEX);
    // Serial.print(" ");
    // Serial.println(value, HEX);

       

    switch (statusByte & 0xF0) {
      case 0x80: // Note Off

        sendESP32Log("USB MIDI IN: NOTE OFF Pitch: " + String(channel) + " Velocity: " + String(value));
            
        turnOffLED(channel);
        break;

      case 0x90: // Note On
        if (value == 0) {

          sendESP32Log("USB MIDI IN: NOTE OFF Pitch: " + String(channel) + " Velocity: " + String(value));
              
          turnOffLED(channel);// Turn off the corresponding LED

        } else {
          
          sendESP32Log("USB MIDI IN: NOTE ON Pitch: " + String(channel) + " Velocity: " + String(value));
              
          lightUpLED(channel, value);  // Set brightness based on velocity 
           
        }
        break;

      case 0xB0: // Control Change
        // Handle Control Change messages if needed

        //from OG Code
        // Process Control Change messages
            switch (channel) {
              case 64: // Sustain Pedal
                sendESP32Log("USB MIDI IN: Sustain Pedal CC " + String(channel) + " Value: " + String(value));
                break;

              case 67: // Soft Pedal
                sendESP32Log("USB MIDI IN: Soft Pedal CC " + String(channel) + " Value: " + String(value));
                break;

              case 66: // Sostenuto Pedal
                sendESP32Log("USB MIDI IN: Sostenuto Pedal CC " + String(channel) + " Value: " + String(value));
                break;
            }
            
        break;
    }
  }
  
}
