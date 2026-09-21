#pragma once
#include <stdint.h>
#include <string.h>

struct PackedRows {
  const uint8_t *data;
  const uint16_t *offsets;
  uint16_t width, height;
  uint8_t wordBytes;
};

// A row is a sequence of run/literal packets, each with at most 128 pixels.
// Only the current scanline is expanded in RAM; source colors are lossless.
template <typename T>
bool decodeRow(const PackedRows &image, uint16_t row, T *output) {
  if (row >= image.height || sizeof(T) != image.wordBytes) return false;
  const uint8_t *p = image.data + image.offsets[row];
  const uint8_t *end = image.data + image.offsets[row + 1];
  uint16_t x = 0;
  while (p < end) {
    const uint8_t control = *p++;
    const uint16_t count = (control & 127) + 1;
    if (x + count > image.width) return false;
    const uint16_t needed = (control & 128 ? 1 : count) * sizeof(T);
    if (end - p < needed) return false;
    if (control & 128) {
      T value = *p++;
      if (sizeof(T) == 2) value |= uint16_t(*p++) << 8;
      for (uint16_t i = 0; i < count; ++i) output[x++] = value;
    } else {
      // SAMD21 is little-endian. Copy literal packets in bulk instead of
      // unpacking each component in the pixel loop.
      memcpy(output + x, p, needed);
      p += needed;
      x += count;
    }
  }
  return x == image.width;
}
