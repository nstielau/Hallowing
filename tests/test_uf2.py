import importlib.util
from pathlib import Path
import struct
import unittest

spec = importlib.util.spec_from_file_location('uf2', Path(__file__).parents[1] / 'tools/uf2.py')
uf2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(uf2)


class FirmwareValidationTests(unittest.TestCase):
    def setUp(self):
        self.binary = struct.pack('<2I', 0x20008000, 0x2101) + bytes(range(256))

    def test_application_roundtrip_and_bootloader_boundary(self):
        memory = uf2.decode(uf2.pack(self.binary))
        self.assertEqual(min(memory), 0x2000)
        self.assertEqual(bytes(memory[i] for i in range(0x2000, 0x2000 + len(self.binary))), self.binary)

    def test_reject_incomplete_backup_and_oversized_application(self):
        for binary in (b'\xff' * 256, self.binary + bytes(0x40000)):
            with self.assertRaises(ValueError):
                uf2.pack(binary)

    def test_reject_duplicate_and_truncated_blocks(self):
        data = uf2.pack(self.binary)
        for invalid in (data[:-1], data[:512] * 2):
            with self.assertRaises(ValueError):
                uf2.decode(invalid)

    def test_reject_other_board_family(self):
        data = bytearray(uf2.pack(self.binary))
        struct.pack_into('<I', data, 8, 0x2000)
        struct.pack_into('<I', data, 28, 0x55114460)
        with self.assertRaises(ValueError):
            uf2.decode(data)
