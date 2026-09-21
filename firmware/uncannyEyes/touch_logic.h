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

// Both blink pads together for 1.5 seconds select the next style. One change
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
private:
  uint32_t startedAt_ = 0;
  bool timing_ = false, latched_ = false;
};
