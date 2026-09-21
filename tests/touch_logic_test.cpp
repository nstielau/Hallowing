#include "../firmware/uncannyEyes/touch_logic.h"
#include <assert.h>
#include <stdio.h>

int main() {
  TouchPad pad;
  pad.begin(400);
  assert(!pad.update(470, 100));
  assert(!pad.update(400, 110)); // Short noise pulse is ignored.
  assert(!pad.update(470, 200));
  assert(!pad.update(470, 224));
  assert(pad.update(470, 225));
  assert(pad.touched());
  for (uint32_t t = 250; t < 10000; t += 25) {
    assert(!pad.update(470, t)); // Sustained hold is not learned away.
    assert(pad.touched());
  }
  assert(pad.baseline() == 400);
  assert(!pad.update(440, 10000)); // Hysteresis keeps a weak hold active.
  assert(pad.touched());
  pad.update(400, 10100);
  pad.update(400, 10125);
  assert(!pad.touched());
  pad.update(470, 10200);
  assert(pad.update(470, 10225)); // Release rearms a new blink event.
  pad.update(65535, 10300);
  pad.update(65535, 10325);
  assert(!pad.touched()); // Failed measurement cannot stick the gaze.

  pad.begin(400);
  pad.update(470, UINT32_MAX - 10);
  assert(pad.update(470, 15)); // millis() wraparound.

  assert(touchEyeX(touchGaze(true, false)) == 0);
  assert(touchEyeX(touchGaze(false, true)) == 1023);
  assert(touchEyeX(touchGaze(true, true)) == 512);
  assert(touchGaze(false, false) == 0);
  puts("Touch debounce, hold, release, retrigger, errors, rollover and gaze passed.");
}
