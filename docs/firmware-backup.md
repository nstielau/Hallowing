# Original firmware and recovery

The board was identified on 2026-09-20 as an Adafruit HalloWing M0, USB VID
`239a`, application PID `dead`, at `/dev/cu.usbmodem1101` on this Mac.
It was running compiled Arduino firmware, not a mounted CircuitPython filesystem.

```text
UF2 Bootloader v2.0.0-adafruit.0-21-g887cc30 SFHWRO
Model: Hallowing M0
Board-ID: SAMD21G18A-Feather-v0
```

## Backup limitation

During initial inspection, a 1200-baud software reset was used to enter the
bootloader. This older SAMD21 Arduino reset implementation erases the first
256-byte application row (`0x2000`–`0x20ff`) before resetting. The backup was
captured after that reset, so **it is not an intact, directly restorable copy
of the original program**. Physical double-reset should have been used instead.
No firmware had been uploaded before the backup was captured.

The captured files remain locally in `backups/original-2026-09-20/`, excluded
from Git. `CURRENT.UF2` was read twice and compared byte-for-byte. Its SHA-256 is:

```text
448c64db20541eaaddc9f7509c274f35914e41e22f5bd7fec85ab30b7101cf92
```

The full graphics arrays match the upstream `defaultEye.h` byte-for-byte:

| Array | Bytes | Offset in captured flash |
| --- | ---: | ---: |
| sclera | 80000 | 27156 |
| iris | 32768 | 152980 |
| symmetrical upper eyelid | 16384 | 136596 |
| symmetrical lower eyelid | 16384 | 107412 |
| polar eye-shape map | 12800 | 123796 |

The complete original binary differs from Adafruit's downloadable stock human-eye
demo. The new firmware preserves the exact artwork using upstream Uncanny Eyes
source; it is not a binary patch of the original program. The new program does
not access the separate 8 MB SPI flash.

## Future backups

Physically double-press Reset, wait for `HALLOWBOOT`, then run `make backup`.
This reads `CURRENT.UF2`, records a checksum, and validates that the application
vector is present. It never triggers a software reset or writes to the board.
Press Reset once to return to the application after a backup.

## Return to the standard eye demo

Adafruit publishes a [stock human-eye UF2](https://learn.adafruit.com/adafruit-hallowing/spooky-eyes).
Physically double-press Reset and copy that official UF2 to `HALLOWBOOT`.
It restores the standard visual demo, **not the exact pre-modification binary**.
To reinstall this repository's touch-enabled version, use `make flash` in
physical bootloader mode.

References: [SAMD21 Arduino reset implementation](https://github.com/adafruit/ArduinoCore-samd/blob/1.7.17/cores/arduino/Reset.cpp),
[UF2 bootloader guide](https://learn.adafruit.com/adafruit-hallowing/uf2-bootloader-details).
