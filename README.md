# Hallowing

Touch controls for the Adafruit HalloWing M0's animated human eye. Built with
Arduino and Adafruit Uncanny Eyes, retaining the original artwork, automatic
movement, blinking, pupil response to light, and eyelid tracking.

## Controls

Facing the screen with the fangs pointing down:

| Pad | Arduino pin | Action |
| --- | --- | --- |
| Left outer fang | A5 | Hold to look left; release to resume movement |
| Left bottom fang | A4 | One complete blink per touch |
| Right bottom fang | A3 | One complete blink per touch |
| Right outer fang | A2 | Hold to look right; release to resume movement |

Holding both outer pads centers the eye. Blinking also works while a direction
is held. A touch during an existing blink queues another blink; holding a bottom
pad does not repeatedly blink or hold the eye shut.

Leave pads untouched during startup calibration (about 150 ms). Touch readings
use per-pad baselines, hysteresis and debounce. If clips or conductive extensions
are added, attach them before powering on. Serial diagnostics show each pad's
raw reading, baseline, and held state, in A5/A4/A3/A2 order.

## Build and load

Requires Python 3, a C++ compiler for tests, and a USB cable that carries data.
Dependencies and Arduino CLI are installed within this checkout:

```sh
make setup
make test
make build
```

Switch the board on and **physically double-press Reset**. When the `HALLOWBOOT`
drive appears, run:

```sh
make backup
make flash
```

The output is `build/Hallowing-touch.uf2`. Flashing validates the board model and
application address range, leaves the bootloader alone, and restarts the board.
It does not touch the separate SPI flash. Do not use Arduino's 1200-baud software
reset before making backups: it can erase the first application row on this M0.
See [original firmware and recovery notes](docs/firmware-backup.md) for the initial
backup limitation and the stock-demo recovery option.

```sh
make monitor PORT=/dev/cu.usbmodem1101
```

USB port names can change. Override the bootloader volume with
`make flash VOLUME=/path/to/HALLOWBOOT` on another OS. This firmware targets the
**M0**, not the M4.

## Validation

The firmware compiles for `adafruit:samd:adafruit_hallowing`. Automated checks cover
touch noise/debounce, long holds, release/retrigger, invalid measurements, timer
rollover, and UF2 validation. The attached board reports all four sensors working
and approximately 32 frames per second. The user verified touch, release-to-resume,
and both blink pads; the initial gaze directions were reversed and then corrected.

Source attribution is in [THIRD_PARTY.md](THIRD_PARTY.md).
