#include "LEDHandler.hpp"

CRGB leds[NUM_LEDS];
int keyLEDStart[NUM_KEYS];
uint8_t noteToColor[88];

bool isWhiteKey(uint8_t note) {
  int whiteNotes[] = {21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107};
  for (int i = 0; i < sizeof(whiteNotes) / sizeof(whiteNotes[0]); i++) {
    if (note == whiteNotes[i]) return true;
  }
  return false;
}

void setupLEDMapping() {
  int ledIndex = 0;
  int keyGroup = 0;
  int noteInGroup = 0;

  for (int note = 21; note <= 108; note++) {
    keyLEDStart[note - 21] = ledIndex;
    noteToColor[note - 21] = map(note, 21, 108, 0, 255);

    if (isWhiteKey(note)) {
      ledIndex += 2;
    } else {
      ledIndex += 1;
    }

    noteInGroup++;
    if (noteInGroup == 3) {
      noteInGroup = 0;
      keyGroup++;
      ledIndex += OCTAVE_GAP;
    }

    if (ledIndex >= NUM_LEDS) {
      ledIndex = NUM_LEDS - 1;
    }
  }
}

void lightUpLED(uint8_t note, uint8_t velocity) {
  if (note < 21 || note > 108) return;

  int ledIndex = keyLEDStart[note - 21];
  CHSV color = CHSV(noteToColor[note - 21], 255, map(velocity, 0, 127, 0, BRIGHTNESS));

  leds[ledIndex] = color;
  if (isWhiteKey(note) && ledIndex + 1 < NUM_LEDS) {
    leds[ledIndex + 1] = color;
  }

  FastLED.show();
}

void turnOffLED(uint8_t note) {
  if (note < 21 || note > 108) return;

  int ledIndex = keyLEDStart[note - 21];

  leds[ledIndex] = CRGB::Black;
  if (isWhiteKey(note) && ledIndex + 1 < NUM_LEDS) {
    leds[ledIndex + 1] = CRGB::Black;
  }

  FastLED.show();
}
