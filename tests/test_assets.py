import importlib.util
from pathlib import Path
import struct
import subprocess
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('build_assets', ROOT/'tools/build_assets.py')
assets = importlib.util.module_from_spec(spec)
spec.loader.exec_module(assets)


class ArtworkTests(unittest.TestCase):
    def test_firmware_decoder_matches_every_source_pixel(self):
        output = ROOT/'build/decoded-assets.bin'
        subprocess.run([str(ROOT/'build/asset_decode_test'), str(output)], check=True)
        expected = bytearray()
        for values, width, height, size, packed in assets.source_assets().values():
            if packed:
                expected.extend(struct.pack('<'+('B' if size == 1 else 'H')*len(values), *values))
        self.assertEqual(output.read_bytes(), expected)

    def test_generation_is_reproducible(self):
        header = ROOT/'firmware/uncannyEyes/graphics/eye_assets.h'
        before = header.read_bytes()
        subprocess.run(['python3', str(ROOT/'tools/build_assets.py')], check=True, stdout=subprocess.DEVNULL)
        self.assertEqual(header.read_bytes(), before)
