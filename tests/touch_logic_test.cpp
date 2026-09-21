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
  EyeStyleChord chord;
  assert(!chord.blocksDilation());
  assert(!chord.update(true, false, 0));
  assert(!chord.update(true, false, 2000)); // One long touch still only blinks.
  assert(!chord.update(true, true, 3000));
  assert(chord.blocksDilation());
  assert(!chord.update(true, true, 4499));
  assert(chord.update(true, true, 4500));
  assert(!chord.update(true, true, 9000)); // No repeated cycling while held.
  assert(!chord.update(false, true, 9100));
  assert(chord.blocksDilation()); // Partial release cannot accidentally adjust size.
  assert(!chord.update(true, true, 12000)); // Partial release does not rearm.
  assert(!chord.update(false, false, 13000));
  assert(!chord.blocksDilation());
  assert(!chord.update(true, true, 14000));
  assert(!chord.update(true, false, 14500)); // Interrupted chord cancels timer.
  assert(!chord.blocksDilation());
  assert(!chord.update(true, true, 16000));
  assert(chord.update(true, true, 17500));
  chord.update(false, false, UINT32_MAX - 2000);
  assert(!chord.update(true, true, UINT32_MAX - 1000));
  assert(chord.update(true, true, 500)); // Timer wraparound also works.
  PupilSweep pupil;
  pupil.update(false, 50000);
  assert(pupil.value(120, 720) == 120);
  pupil.update(true, 60000); // First touch excludes all previous idle time.
  assert(pupil.value(120, 720) == 120);
  pupil.update(true, 62000);
  assert(pupil.value(120, 720) == 420);
  assert(pupil.value(80, 400) == 240); // Style changes retain relative size.
  assert(pupil.value(80, 320) == 200);
  pupil.update(true, 64000);
  assert(pupil.value(120, 720) == 720);
  pupil.update(true, 66000);
  assert(pupil.value(120, 720) == 420);
  pupil.update(false, 66010);
  pupil.update(false, 90000);
  pupil.update(true, 100000);
  assert(pupil.value(120, 720) == 420); // Release freezes size and direction.
  pupil.update(true, 101000);
  assert(pupil.value(120, 720) == 270); // Resumes shrinking.
  pupil.update(true, 102000);
  assert(pupil.value(120, 720) == 120);
  pupil.update(true, 114000);
  assert(pupil.value(120, 720) == 720); // Repeats across multiple periods.

  PupilSweep wrappedPupil;
  wrappedPupil.update(true, UINT32_MAX - 1999);
  wrappedPupil.update(true, 2000);
  assert(wrappedPupil.value(120, 720) == 720); // millis() wraparound.
  wrappedPupil.update(true, 6000);
  assert(wrappedPupil.value(120, 720) == 120);
  puts("Touch, gaze, style-chord priority and manual pupil sweep checks passed.");
}
