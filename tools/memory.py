#!/usr/bin/env python3
"""Report application headroom, excluding the HalloWing's 8 KiB bootloader."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
size_tool = root/'.arduino/data/packages/adafruit/tools/arm-none-eabi-gcc/9-2019q4/bin/arm-none-eabi-size'
elf = root/'build/eye/uncannyEyes.ino.elf'
report = subprocess.check_output([str(size_tool), '-A', str(elf)], text=True)
sections = {}
for line in report.splitlines():
    parts = line.split()
    if len(parts) >= 2 and parts[0] in ('.text', '.data', '.bss'):
        sections[parts[0]] = int(parts[1])
flash_used = (root/'build/eye/uncannyEyes.ino.bin').stat().st_size
ram_static = sections['.data'] + sections['.bss']
print(f'Program flash: {flash_used:,} / 253,952 bytes; {253952-flash_used:,} bytes ({(253952-flash_used)/1024:.1f} KiB) free')
print(f'Static RAM: {ram_static:,} / 32,768 bytes; {32768-ram_static:,} bytes ({(32768-ram_static)/1024:.1f} KiB) before stack/heap')
print('Bootloader: 8,192 bytes reserved. Separate 8 MB SPI flash is not used by this firmware.')
