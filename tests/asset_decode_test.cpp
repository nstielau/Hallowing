#include "../firmware/uncannyEyes/eye_art.h"
#include <assert.h>
#include <stdio.h>
#include <initializer_list>

int main(int argc, char **argv) {
  assert(argc == 2);
  FILE *file = fopen(argv[1], "wb");
  assert(file);
  const PackedRows *images[] = {&human_sclera, &upper_lid, &lower_lid,
                               &human_polar, &lizard_polar, &goat_polar};
  for (const PackedRows *image : images) {
    for (uint16_t y = 0; y < image->height; ++y) {
      if (image->wordBytes == 1) {
        uint8_t row[200];
        assert(decodeRow(*image, y, row));
        assert(fwrite(row, 1, image->width, file) == image->width);
      } else {
        uint16_t row[200];
        assert(decodeRow(*image, y, row));
        assert(fwrite(row, 2, image->width, file) == image->width);
      }
    }
  }
  fclose(file);
  uint16_t row[200];
  assert(!decodeRow(human_sclera, 200, row));
  assert(!decodeRow(upper_lid, 0, row));
  // Run that overflows its output row must fail instead of overwriting RAM.
  const uint8_t badData[] = {0xff, 0x00};
  const uint16_t offsets[] = {0, 2};
  const PackedRows malformed = {badData, offsets, 1, 1, 1};
  uint8_t byte = 42;
  assert(!decodeRow(malformed, 0, &byte));
  assert(byte == 42);
  // Exercise each style's scanline buffers at every legal gaze extreme.
  for (unsigned style = 0; style < eyeStyleCount; ++style) {
    const int width = SCLERA_WIDTH;
    for (int gazeY : {0, width - 128}) {
      const int irisStart = gazeY - (width - IRIS_HEIGHT)/2;
      for (int y = 0; y < 128; ++y) prepareEyeRow(y, gazeY+y, irisStart+y);
    }
    nextEyeStyle();
  }
  assert(activeEyeStyle == 0);
  puts("All packed rows decoded; style buffers and malformed packet checks passed.");
}
