// Pin selections here are based on the original Adafruit Learning System
// guide for the Teensy 3.x project.  Some of these pin numbers don't even
// exist on the smaller SAMD M0 & M4 boards, so you may need to make other
// selections:

// GRAPHICS SETTINGS (appearance of eye) -----------------------------------

// If using a SINGLE EYE, you might want this next line enabled, which
// uses a simpler "football-shaped" eye that's left/right symmetrical.
// Default shape includes the caruncle, creating distinct left/right eyes.
// Hallowing, with one eye, does this by default
#if defined(ADAFRUIT_HALLOWING) || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(ARDUINO_NRF52840_CIRCUITPLAY)
  #define SYMMETRICAL_EYELID
#else                     // Otherwise your choice, standard is asymmetrical
  //#define SYMMETRICAL_EYELID
#endif

// Three styles share losslessly compressed scanline artwork. The original
// defaultEye.h remains the build-time source, not a second compiled copy.
#include "eye_art.h"

// Optional: enable this line for startup logo (screen test/orient):
#if !defined(ADAFRUIT_HALLOWING)    // Hallowing can't always fit logo+eye
  #include "graphics/logo.h"        // Otherwise your choice, if it fits
#endif

// EYE LIST ----------------------------------------------------------------

// This table contains ONE LINE PER EYE.  The table MUST be present with
// this name and contain ONE OR MORE lines.  Each line contains THREE items:
// a pin number for the corresponding TFT/OLED display's SELECT line, a pin
// pin number for that eye's "wink" button (or -1 if not used), and a screen
// rotation value (0-3) for that eye.

eyeInfo_t eyeInfo[] = {
#if defined(ADAFRUIT_HALLOWING)
  { 39, -1, 2 }, // SINGLE EYE display-select and wink pins, rotate 180
#elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(ARDUINO_NRF52840_CIRCUITPLAY)
  { A6, -1, 2 }, // SINGLE EYE display-select and wink pins, rotate 180
#elif defined(ADAFRUIT_TRINKET_M0)
  {  0, -1, 0 }, // SINGLE EYE display-select, no wink, no rotation
#else
  {  9, 0, 0 }, // LEFT EYE display-select and wink pins, no rotation
  { 10, 2, 0 }, // RIGHT EYE display-select and wink pins, no rotation
#endif
};

// DISPLAY HARDWARE SETTINGS (screen type & connections) -------------------

#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
  #define TFT_SPI        SPI1
  #define TFT_PERIPH     PERIPH_SPI1
#else
  #define TFT_SPI        SPI
  #define TFT_PERIPH     PERIPH_SPI
#endif

#if defined(ADAFRUIT_HALLOWING)
  #include <Adafruit_ST7735.h>  // 128x128 TFT display library
  #define DISPLAY_DC        38  // Display data/command pin
  #define DISPLAY_RESET     37  // Display reset pin
  #define DISPLAY_BACKLIGHT  7
  #define BACKLIGHT_MAX    128
  //#define SYNCPIN         A2  // I2C sync if set, GND this pin on receiver
  //#define SYNCADDR      0x08  // I2C address of receiver
                                // (Try disabling SYMMETRICAL_EYELID then)
#elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(ARDUINO_NRF52840_CIRCUITPLAY)
  #include <Adafruit_ST7789.h> // 240x240 TFT display library
  #define DISPLAY_DC        A7 // Display data/command pin
  #define DISPLAY_RESET     -1 // Display reset pin
  #define DISPLAY_BACKLIGHT A3
  #define BACKLIGHT_MAX    255
#else
  // Enable ONE of these #includes to specify the display type being used
  //#include <Adafruit_SSD1351.h>  // OLED display library -OR-
  #include <Adafruit_ST7735.h>  // TFT display library (enable one only)
  #if defined(ADAFRUIT_TRINKET_M0)
    #define DISPLAY_DC       1
    #define DISPLAY_RESET   -1 // Use MCU reset pin
  #else
    #define DISPLAY_DC       7  // Data/command pin for ALL displays
    #define DISPLAY_RESET    8  // Reset pin for ALL displays
  #endif
#endif

#if defined(_ADAFRUIT_ST7735H_) || defined(_ADAFRUIT_ST77XXH_)
 #if defined(ARDUINO_ARCH_NRF52)
  #define SPI_FREQ 32000000    // TFT: use max SPI
 #else
  #define SPI_FREQ 24000000    // TFT: use max SPI
 #endif
#else // OLED
  #if !defined(ARDUINO_ARCH_SAMD) && (F_CPU <= 72000000)
    #define SPI_FREQ 24000000  // OLED: 24 MHz on 72 MHz Teensy only
  #else
    #define SPI_FREQ 12000000  // OLED: 12 MHz in all other cases
  #endif
#endif

// INPUT SETTINGS (for controlling eye motion) -----------------------------

// JOYSTICK_X_PIN and JOYSTICK_Y_PIN specify analog input pins for manually
// controlling the eye with an analog joystick.  If set to -1 or if not
// defined, the eye will move on its own.
// Pupil size is controlled by the right middle capacitive pad in touch_control.h.
// BLINK_PIN specifies an input pin for a button (to ground) that will
// make any/all eyes blink.  If set to -1 or if not defined, the eyes will
// only blink if AUTOBLINK is defined, or if the eyeInfo[] table above
// includes wink button settings for each eye.

//#define JOYSTICK_X_PIN A0 // Analog pin for eye horiz pos (else auto)
//#define JOYSTICK_Y_PIN A1 // Analog pin for eye vert position (")
//#define JOYSTICK_X_FLIP   // If defined, reverse stick X axis
//#define JOYSTICK_Y_FLIP   // If defined, reverse stick Y axis
#define TRACKING            // If defined, eyelid tracks pupil
#define AUTOBLINK           // If defined, eyes also blink autonomously
// Capacitive blink/style/pupil controls are handled in touch_control.h.
// The covered light sensor is deliberately unused.
#define BLINK_PIN -1
