
// +---- PROJECT SPHERE LAMP v1.0 -------+
// |                                     |
// |  Author: Fabio Corona - 2021        |
// |                                     |
// +-------------------------------------+


// ---- LIBRARIES IMPORTS ---- //

#include <PololuLedStrip.h>
#include "DHT.h"

// PololuLed Variables / Inits:
PololuLedStrip<7> ledStrip;
#define LED_COUNT 4
rgb_color colors[LED_COUNT];

// DHT11 Variables / Inits:
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);



// -------- PROGRAM GLOBAL VARIABLES -------- //

// MODE 0 = Full Bright
// MODE 1 = Pulse
// MODE 2 = Gradient
// MODE 3 = Magic
// MODE 4 = Temperature
unsigned int mode = 0; // Init
const unsigned int maxModes = 5;

// Pin Assignments:
int btnMode = 2;
int btnColor = 3;

// Push Buttons Variables [ Init ]:
int pushValue_one = 0;
int pushState_one = 0; // LOW
int pushLast_one = 0;

int pushValue_two = 0;
int pushState_two = 0; // LOW
int pushLast_two = 0;

unsigned long prevMs = 0;
const long interval = 5;



// ---- AUXILIARY FUNCTIONS ---- //

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t p = (255 - s) * (uint16_t)v / 255;
  uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
  uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
  uint8_t r = 0, g = 0, b = 0;
  switch ((h / 60) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
  return rgb_color(r, g, b);
}



// -------- SETUP -------- //

void setup() {
  pinMode(btnMode, INPUT);
  pinMode(btnColor, INPUT);
  Serial.begin(9600);
  dht.begin();
}



// -------- MAIN LOOP -------- //

void loop() {
  // Change Detection
  // [ https://www.arduino.cc/en/Tutorial/BuiltInExamples/StateChangeDetection ]
  pushState_one = digitalRead(btnMode);
  if (pushState_one != pushLast_one) {
    if (pushState_one == 1) {

      // Reset some values after MODE CHANGE:
      pushBtnReset_two(); // CALL PUSH BTN TWO RESET TO CLEAR PUSH BTN STATUS "RE-INIT VALUES"
      ledReset(); // CALL LED RESET FUNCTION TO CLEAR LED PREVIOUS STATE

      pushValue_one++;
    }
    delay(50);
  }
  pushLast_one = pushState_one;
  //

  // Reset modes after going to last mode. (out of range).
  if (pushValue_one > maxModes - 1) {
    pushValue_one = 0;
  }

  mode = pushValue_one;

  switch (mode) {
    case 0:
      full_bright();
      break;
    case 1:
      pulse();
      break;
    case 2:
      gradient();
      break;
    case 3:
      magic();
      break;
    case 4:
      temp();
      delay(250); // TEMP Sensor delay Fix IDK if it works...
      break;
  }
}



// -------- FULL BRIGHT MODE -------- //

void full_bright() {

  // Change Detection
  // [ https://www.arduino.cc/en/Tutorial/BuiltInExamples/StateChangeDetection ]
  pushState_two = digitalRead(btnColor);
  if (pushState_two != pushLast_two) {
    if (pushState_two == 1) {
      pushValue_two = pushValue_two + 40;
    }
    delay(50);
  }
  pushLast_two = pushState_two;
  //

  // Reset HUE if out of range:
  if (pushValue_two == 360) {
    pushValue_two = 0;
  }

  int fb_color = pushValue_two;

  for (int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = hsvToRgb(fb_color, 255, 255);
  }
  ledStrip.write(colors, LED_COUNT);
}



// -------- PULSE MODE -------- //

// Pulse Variable - Init Values
unsigned int v1 = 0;
unsigned int loopState1 = 0; // 0 = Forward / 1 = Backwards
unsigned int loopCounter1 = 0;
unsigned int loopLedSeq1 = 0; // Led Index

void pulse() {
  // Change Detection
  // [ https://www.arduino.cc/en/Tutorial/BuiltInExamples/StateChangeDetection ]
  pushState_two = digitalRead(btnColor);
  if (pushState_two != pushLast_two) {
    if (pushState_two == 1) {
      pushValue_two = pushValue_two + 40;
    }
    delay(50);
  }
  pushLast_two = pushState_two;
  //

  // Reset HUE if out of range:
  if (pushValue_two == 360) {
    pushValue_two = 0;
  }

  int pulse_color = pushValue_two;

  const unsigned int s = 255; // Max Saturation
  const unsigned int maxValue = 255;
  unsigned long currentMs = millis();
  if (currentMs - prevMs >= interval) {
    prevMs = currentMs;
    // DO:
    if (loopState1 == 0) {
      v1++;
    } else if (loopState1 == 1) {
      v1--;
    }
    if (v1 == maxValue) {
      loopState1 = 1;
    } else if (v1 == 0) {
      loopState1 = 0;
    }

    for (int i = 0; i < LED_COUNT; i++) {
      colors[i] = hsvToRgb(pulse_color, s, v1);
    }
    ledStrip.write(colors, LED_COUNT);
  }
}



// -------- GRADIENT MODE -------- //

// Gradient Variables - Init Values
unsigned int v = 0;
unsigned int loopState = 0; // 0 = Forward / 1 = Backwards
unsigned int loopCounter = 0;
unsigned int loopLedSeq = 0; // Led Index

void gradient() {
  // Change Detection
  // [ https://www.arduino.cc/en/Tutorial/BuiltInExamples/StateChangeDetection ]
  pushState_two = digitalRead(btnColor);
  if (pushState_two != pushLast_two) {
    if (pushState_two == 1) {
      pushValue_two = pushValue_two + 40;
    }
    delay(50);
  }
  pushLast_two = pushState_two;
  //

  // Reset HUE if out of range:
  if (pushValue_two == 360) {
    pushValue_two = 0;
  }

  int gradient_color = pushValue_two;

  const unsigned int s = 255; // Max Saturation
  const unsigned int maxValue = 255;
  unsigned long currentMs = millis();
  if (currentMs - prevMs >= interval) {
    prevMs = currentMs;
    // DO:
    if (loopState == 0) {
      v++;
    } else if (loopState == 1) {
      v--;
    }
    if (v == maxValue) {
      loopState = 1;
    } else if (v == 0) {
      loopState = 0;
    }

    if (loopCounter == 0) {
      loopLedSeq = 0;
    } else if (loopCounter == maxValue * 2) {
      loopLedSeq = 1;
    } else if (loopCounter == maxValue * 4) {
      loopLedSeq = 2;
    } else if (loopCounter == maxValue * 6) {
      loopLedSeq = 3;
    } else if (loopCounter == maxValue * 8) { // Restart / Reset
      loopLedSeq = 0;
      loopCounter = 0;
    }

    loopCounter++;

    colors[loopLedSeq] = hsvToRgb(gradient_color, s, v);

    ledStrip.write(colors, LED_COUNT);
  }
}



// -------- MAGIC MODE -------- //

// Magic Variables - Init Values
unsigned int v3 = 0;
unsigned int loopState3 = 0; // 0 = Forward / 1 = Backwards
unsigned int loopPos = 0;

// Magic Sequence Variables:
unsigned int seq = 0;
const int magicSeq[] = {0, 1, 2, 3, 2, 1};
unsigned int maxSeq = sizeof(magicSeq) / sizeof(int); // magicSeq Array Lenght

// Magic Palettes Variables:
unsigned int hMagic = 0;
unsigned int hSeq = 0;
// Palette LIST: [ Thanks to https://colordesigner.io/ ]
const int hPaletteListSize = 8; // Numbers of Palettes
const int hPalette1[] = {0, 150, 210};
const int hPalette2[] = {60, 210, 270};
const int hPalette3[] = {100, 250, 310};
const int hPalette4[] = {150, 300, 360};
const int hPalette5[] = {190, 340, 40};
const int hPalette6[] = {230, 20, 80};
const int hPalette7[] = {280, 70, 130};
const int hPalette8[] = {320, 110, 170};
const unsigned int maxPaletteSize = 3; // Max colors in palettes.

void magic() {
  // Change Detection
  // [ https://www.arduino.cc/en/Tutorial/BuiltInExamples/StateChangeDetection ]
  pushState_two = digitalRead(btnColor);
  if (pushState_two != pushLast_two) {
    if (pushState_two == 1) {
      pushValue_two++;
    }
    delay(50);
  }
  pushLast_two = pushState_two;
  //

  const unsigned int s = 255; // Max Saturation
  const unsigned int maxValue = 255;

  unsigned long currentMs = millis();
  if (currentMs - prevMs >= interval) {
    prevMs = currentMs;
    // DO:
    if (loopState3 == 0) {
      v3++;
    } else if (loopState3 == 1) {
      v3--;
    }
    if (v3 == maxValue) {
      loopState3 = 1;
    } else if (v3 == 0) {
      loopState3 = 0;
    }

    // Main Magic Loop:
    if (loopPos == 0) {
      // Loop Start //
    } else if (loopPos == maxValue * 2) {
      // On Loop End DO:

      // Sequence handler:
      if (seq == maxSeq - 1) {
        seq = 0;
      } else {
        seq++;
      }
      loopPos = 0;

      // Palette Handler:
      if (hSeq == maxPaletteSize - 1) {
        hSeq = 0;
      } else {
        hSeq++;
      }
    }

    // Palette Switcher:
    if (pushValue_two == hPaletteListSize) {
      pushValue_two = 0;
    }
    switch (pushValue_two) {
      case 0: hMagic = hPalette1[hSeq]; break;
      case 1: hMagic = hPalette2[hSeq]; break;
      case 2: hMagic = hPalette3[hSeq]; break;
      case 3: hMagic = hPalette4[hSeq]; break;
      case 4: hMagic = hPalette5[hSeq]; break;
      case 5: hMagic = hPalette6[hSeq]; break;
      case 6: hMagic = hPalette7[hSeq]; break;
      case 7: hMagic = hPalette8[hSeq]; break;
    }


    loopPos++;

    colors[magicSeq[seq]] = hsvToRgb(hMagic, s, v3);
    ledStrip.write(colors, LED_COUNT);
  }
}



// -------- TEMP MODE -------- //

unsigned int temp_color;
const float coldTemp = 10.00;
const float warmTemp = 30.00;

// Temperature Color Palette:
// TEMP:            COL:  PERC:

// Freeze:          270   [0%]-[15%]
// Very Cold:       230   [15%]-[30%]
// Cold:            200   [30%]-[40%]

// Normal/Optimal:  125   [45%]-[55%]

// Warm:            65    [60%]-[70%]
// Very Warm:       35    [70%]-[85%]
// Hot:             0     [85%]-[100%]

const int tempPalette[] = {270, 230, 200, 125, 65, 35, 0};

void temp() {

  // Read Temperature
  float t = dht.readTemperature();
  // Remap temperature
  int tempMap = map(t, coldTemp, warmTemp, 0, 100);
  if (tempMap >= 0 && tempMap <= 15) {
    temp_color = tempPalette[0];
  } else if (tempMap >= 15 && tempMap <= 30) {
    temp_color = tempPalette[1];
  } else if (tempMap >= 30 && tempMap <= 40) {
    temp_color = tempPalette[2];
  } else if (tempMap >= 45 && tempMap <= 55) {
    temp_color = tempPalette[3];
  } else if (tempMap >= 60 && tempMap <= 70) {
    temp_color = tempPalette[4];
  } else if (tempMap >= 70 && tempMap <= 85) {
    temp_color = tempPalette[5];
  } else if (tempMap >= 85 && tempMap <= 100) {
    temp_color = tempPalette[6];
  }

  for (int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = hsvToRgb(temp_color, 255, 255);
  }
  ledStrip.write(colors, LED_COUNT);
}



// ---- MODES RESET LEDS ---- //

void ledReset() {
  for (int i = 0; i < LED_COUNT; i++) {
    colors[i] = rgb_color(0, 0, 0);
  }
  ledStrip.write(colors, LED_COUNT);
}



// ---- PUSH BTN TWO RESET ---- //

void pushBtnReset_two() {
  pushValue_two = 0;
  pushState_two = 0; // LOW
  pushLast_two = 0;
}
