#pragma once
#include "packed_rows.h"
#include "graphics/eye_assets.h"

struct EyeStyle {
  const char *name;
  const uint16_t *iris;
  const PackedRows *polar;
  uint16_t mapWidth, mapHeight, diameter, scleraWidth;
  uint16_t pupilMin, pupilMax;
};

static const EyeStyle eyeStyles[] = {
  {"human", human_iris, &human_polar, 256, 64, 80, 200, 120, 720},
  {"lizard", lizard_iris, &lizard_polar, 256, 32, 128, 160, 80, 400},
  {"goat", goat_iris, &goat_polar, 256, 32, 128, 160, 80, 320}
};
static uint8_t activeEyeStyle = 0;
static const uint8_t eyeStyleCount = sizeof(eyeStyles)/sizeof(eyeStyles[0]);
static const EyeStyle &eyeStyle() { return eyeStyles[activeEyeStyle]; }

static uint16_t scleraRow[200], polarRow[128];
static uint8_t upperRow[128], lowerRow[128];
static int16_t upperCachedY = -1;

static void nextEyeStyle() {
  activeEyeStyle = (activeEyeStyle + 1) % eyeStyleCount;
}

static uint8_t upperPixel(uint16_t y, uint16_t x) {
  if (upperCachedY != int16_t(y)) {
    decodeRow(upper_lid, y, upperRow);
    upperCachedY = y;
  }
  return upperRow[x];
}

static void prepareEyeRow(uint16_t screenY, uint16_t scleraY, int16_t irisY) {
  upperPixel(screenY, 0);
  decodeRow(lower_lid, screenY, lowerRow);
  if (activeEyeStyle == 0) decodeRow(human_sclera, scleraY, scleraRow);
  if (irisY >= 0 && irisY < eyeStyle().diameter) decodeRow(*eyeStyle().polar, irisY, polarRow);
}

static uint16_t scleraPixel(uint16_t x) {
  // The upstream reptile/goat designs use a black surround and a larger iris.
  return activeEyeStyle == 0 ? scleraRow[x] : 0;
}

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define SCLERA_WIDTH (eyeStyle().scleraWidth)
#define SCLERA_HEIGHT (eyeStyle().scleraWidth)
#define IRIS_WIDTH (eyeStyle().diameter)
#define IRIS_HEIGHT (eyeStyle().diameter)
#define IRIS_MAP_WIDTH (eyeStyle().mapWidth)
#define IRIS_MAP_HEIGHT (eyeStyle().mapHeight)
#define IRIS_MIN (eyeStyle().pupilMin)
#define IRIS_MAX (eyeStyle().pupilMax)
