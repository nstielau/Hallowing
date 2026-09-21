#pragma once
#include <stdint.h>

// FreeTouch readings are 10-bit. Separate press/release thresholds and a
// short debounce suppress noise without learning away a sustained touch.
class TouchPad {
public:
  void begin(uint16_t baseline) {
    baselineQ8_ = static_cast<int32_t>(baseline) * 256;
    touched_ = candidate_ = false;
    changedAt_ = 0;
  }

  bool update(uint16_t raw, uint32_t now) {
    const bool valid = raw <= 1023;
    const int32_t delta = static_cast<int32_t>(raw) - baseline();
    const bool next = valid && delta >= (touched_ ? 30 : 60);
    if (next != candidate_) {
      candidate_ = next;
      changedAt_ = now;
    }
    if (candidate_ != touched_ && uint32_t(now - changedAt_) >= 25) {
      touched_ = candidate_;
      return touched_; // One press event, never repeated while held.
    }
    if (valid && !touched_ && !candidate_ && delta < 30) {
      baselineQ8_ += (static_cast<int32_t>(raw) * 256 - baselineQ8_) / 128;
    }
    return false;
  }

  bool touched() const { return touched_; }
  uint16_t baseline() const { return baselineQ8_ / 256; }

private:
  int32_t baselineQ8_ = 0;
  uint32_t changedAt_ = 0;
  bool touched_ = false, candidate_ = false;
};

// Viewer-facing left/right. Opposing touches hold the eye centered.
inline int8_t touchGaze(bool left, bool right) {
  return (right ? 1 : 0) - (left ? 1 : 0);
}

// Viewer-facing direction verified on the HalloWing's rotated display.
inline int16_t touchEyeX(int8_t gaze) {
  return gaze < 0 ? 0 : (gaze > 0 ? 1023 : 512);
}

// Both middle pads together for 1.5 seconds select the next style. One change
// per gesture: both pads must be released before the next long press can fire.
class EyeStyleChord {
public:
  bool update(bool left, bool right, uint32_t now) {
    if (!left && !right) {
      timing_ = latched_ = false;
      return false;
    }
    if (latched_) return false;
    if (!left || !right) {
      timing_ = false;
      return false;
    }
    if (!timing_) {
      timing_ = true;
      startedAt_ = now;
    }
    if (uint32_t(now - startedAt_) >= 1500) {
      latched_ = true;
      return true;
    }
    return false;
  }
  bool blocksDilation() const { return timing_ || latched_; }
private:
  uint32_t startedAt_ = 0;
  bool timing_ = false, latched_ = false;
};

// Hold to sweep min -> max -> min, four seconds in each direction. Release
// freezes both size and travel direction; no wall-clock catch-up on the next hold.
// A normalized phase keeps the selected relative size when eye styles change.
class PupilSweep {
public:
  void update(bool held, uint32_t now) {
    if (held && wasHeld_) {
      phase_ = (phase_ + uint32_t(now - lastUpdate_) % 8000) % 8000;
    }
    lastUpdate_ = now;
    wasHeld_ = held;
  }
  uint16_t value(uint16_t minimum, uint16_t maximum) const {
    const uint32_t amount = phase_ <= 4000 ? phase_ : 8000 - phase_;
    return minimum + uint32_t(maximum - minimum) * amount / 4000;
  }
private:
  uint32_t phase_ = 0, lastUpdate_ = 0;
  bool wasHeld_ = false;
};
