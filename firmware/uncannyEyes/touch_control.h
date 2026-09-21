#pragma once
#include <Arduino.h>
#include <Adafruit_FreeTouch.h>
#include "touch_logic.h"

#if !defined(ADAFRUIT_HALLOWING)
#error This sketch targets the HalloWing M0.
#endif

// Facing the screen, fangs down: A5, A4, A3, A2 (the PCB diagram is rear-facing).
static Adafruit_FreeTouch touchSensors[] = {
  Adafruit_FreeTouch(A5, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
  Adafruit_FreeTouch(A4, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
  Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE),
  Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE)
};
static TouchPad touchPads[4];
static bool touchEnabled[4] = {};
static uint16_t touchRaw[4] = {};
static bool touchBlinkPending = false;
static bool touchStylePending = false;
static EyeStyleChord touchStyleChord;

static void touchSetup() {
  // Leave the pads untouched during the brief startup calibration.
  for (uint8_t i = 0; i < 4; ++i) {
    touchEnabled[i] = touchSensors[i].begin();
    uint32_t sum = 0;
    uint8_t count = 0;
    if (touchEnabled[i]) {
      for (uint8_t sample = 0; sample < 16; ++sample) {
        uint16_t raw = touchSensors[i].measure();
        if (raw <= 1023) { sum += raw; ++count; }
        delay(2);
      }
    }
    touchEnabled[i] = count > 0;
    touchPads[i].begin(count ? sum / count : 0);
  }
}

static void touchUpdate() {
  static uint32_t lastSample = 0, lastReport = 0;
  const uint32_t now = millis();
  if (uint32_t(now - lastSample) < 20) return;
  lastSample = now;
  for (uint8_t i = 0; i < 4; ++i) {
    if (!touchEnabled[i]) continue;
    touchRaw[i] = touchSensors[i].measure();
    const bool pressed = touchPads[i].update(touchRaw[i], now);
    if ((i == 1 || i == 2) && pressed) touchBlinkPending = true;
  }
  if (touchStyleChord.update(touchPads[1].touched(), touchPads[2].touched(), now)) {
    touchStylePending = true;
  }
  // Opt-in by opening the serial monitor; never wait for a USB connection.
  if (Serial && uint32_t(now - lastReport) >= 1000 && Serial.availableForWrite() >= 32) {
    lastReport = now;
    Serial.print("TOUCH A5,A4,A3,A2 raw/base/held:");
    for (uint8_t i = 0; i < 4; ++i) {
      Serial.print(' '); Serial.print(touchRaw[i]);
      Serial.print('/'); Serial.print(touchPads[i].baseline());
      Serial.print('/'); Serial.print(touchPads[i].touched());
      if (!touchEnabled[i]) Serial.print('!');
    }
    Serial.println();
  }
}

static bool touchGazeHeld() {
  return touchPads[0].touched() || touchPads[3].touched();
}

static int16_t touchGazeX() {
  return touchEyeX(touchGaze(touchPads[0].touched(), touchPads[3].touched()));
}
