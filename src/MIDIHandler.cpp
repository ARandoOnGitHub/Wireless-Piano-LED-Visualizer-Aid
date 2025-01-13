#include "MIDIHandler.hpp"
#include "LEDHandler.hpp"

void processMIDI(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i += 4) {
    if ((data[i] + data[i + 1] + data[i + 2] + data[i + 3]) == 0) break;

    uint8_t statusByte = data[i + 1];
    uint8_t channel = data[i + 2];
    uint8_t value = data[i + 3];

    switch (statusByte & 0xF0) {
      case 0x80: // Note Off
        turnOffLED(channel);
        break;

      case 0x90: // Note On
        if (value == 0) {
          turnOffLED(channel);
        } else {
          lightUpLED(channel, value);
        }
        break;

      case 0xB0: // Control Change
        // Handle Control Change messages if needed
        break;
    }
  }
}
