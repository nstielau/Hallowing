#!/usr/bin/env python3
"""Pack and inspect HalloWing M0 application UF2s; flash only in physical bootloader mode."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import struct
from datetime import datetime

APP_START = 0x2000
FLASH_END = 0x40000
MAGIC = (0x0A324655, 0x9E5D5157, 0x0AB16F30)


def decode(data):
    if not data or len(data) % 512:
        raise ValueError("UF2 size must be a nonzero multiple of 512")
    memory = {}
    seen = set()
    expected = len(data) // 512
    for offset in range(0, len(data), 512):
        block = data[offset:offset + 512]
        m0, m1, flags, address, size, number, count, family = struct.unpack_from('<8I', block)
        end_magic = struct.unpack_from('<I', block, 508)[0]
        if (m0, m1, end_magic) != MAGIC or flags not in (0, 0x2000):
            raise ValueError("Unsupported or invalid UF2 block")
        if flags == 0x2000 and family != 0x68ED2B88:
            raise ValueError("UF2 is not for SAMD21")
        if size != 256 or address % 256 or address + size > FLASH_END:
            raise ValueError("Invalid SAMD21 flash address/size")
        if number in seen or count != expected or number >= count:
            raise ValueError("Duplicate/missing UF2 block")
        seen.add(number)
        for i, byte in enumerate(block[32:32 + size]):
            if address + i in memory:
                raise ValueError("Overlapping UF2 blocks")
            memory[address + i] = byte
    return memory


def has_app_vector(memory):
    vector = bytes(memory.get(APP_START + i, 255) for i in range(8))
    stack, reset = struct.unpack('<2I', vector)
    return 0x20000000 < stack <= 0x20008000 and APP_START <= (reset & ~1) < FLASH_END and bool(reset & 1)


def pack(binary):
    if not binary or len(binary) > FLASH_END - APP_START:
        raise ValueError("Application will not fit beside the 8 KiB bootloader")
    count = (len(binary) + 255) // 256
    blocks = []
    for number in range(count):
        chunk = binary[number * 256:(number + 1) * 256].ljust(256, b'\xff')
        header = struct.pack('<8I', MAGIC[0], MAGIC[1], 0, APP_START + number * 256, 256, number, count, 0)
        blocks.append(header + chunk + bytes(220) + struct.pack('<I', MAGIC[2]))
    data = b''.join(blocks)
    if not has_app_vector(decode(data)):
        raise ValueError("Binary has no valid SAMD21 application vector")
    return data


def board_info(volume):
    info = (volume / 'INFO_UF2.TXT').read_text()
    if 'Model: Hallowing M0' not in info:
        raise ValueError("Target is not a HalloWing M0 bootloader")
    return info


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('pack')
    p.add_argument('binary', type=Path)
    p.add_argument('output', type=Path)
    p = sub.add_parser('flash')
    p.add_argument('uf2', type=Path)
    p.add_argument('--volume', type=Path, default=Path('/Volumes/HALLOWBOOT'))
    p = sub.add_parser('backup')
    p.add_argument('--volume', type=Path, default=Path('/Volumes/HALLOWBOOT'))
    args = parser.parse_args()
    if args.command == 'pack':
        data = pack(args.binary.read_bytes())
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
        print(f'{args.output}: {len(data)} bytes; application only, starts at 0x2000')
    elif args.command == 'flash':
        board_info(args.volume)
        data = args.uf2.read_bytes()
        memory = decode(data)
        if min(memory) < APP_START or not has_app_vector(memory):
            raise ValueError("Refusing bootloader writes or an incomplete application")
        shutil.copyfile(args.uf2, args.volume / args.uf2.name)
        print('Copied application to HalloWing M0. Board should restart; verify via serial/display.')
    else:
        info = board_info(args.volume)
        data = (args.volume / 'CURRENT.UF2').read_bytes()
        memory = decode(data)
        destination = Path('backups') / datetime.now().strftime('%Y%m%d-%H%M%S-%f')
        destination.mkdir(parents=True)
        (destination / 'CURRENT.UF2').write_bytes(data)
        (destination / 'INFO_UF2.TXT').write_text(info)
        valid = has_app_vector(memory)
        (destination / 'manifest.json').write_text(json.dumps({
            'sha256': hashlib.sha256(data).hexdigest(),
            'valid_application_vector': valid,
            'note': 'Use physical double-reset before backup, never a 1200-baud reset.'
        }, indent=2) + '\n')
        print(destination)
        if not valid:
            raise ValueError("Backup saved, but application vector is invalid; not directly restorable")
        print('Backup saved with valid application vector.')


if __name__ == '__main__':
    main()
