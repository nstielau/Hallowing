CLI := .tools/arduino-cli/arduino-cli --config-file .arduino/arduino-cli.yaml
FQBN := adafruit:samd:adafruit_hallowing
PORT ?= /dev/cu.usbmodem1101
VOLUME ?= /Volumes/HALLOWBOOT

.PHONY: setup assets build test flash flash-serial backup monitor memory
setup:
	bash tools/setup.sh

assets:
	python3 tools/build_assets.py

build:
	$(CLI) compile --fqbn $(FQBN) --build-path build/eye firmware/uncannyEyes
	python3 tools/uf2.py pack build/eye/uncannyEyes.ino.bin build/Hallowing-touch.uf2

test:
	mkdir -p build
	$(CXX) -std=c++11 -Wall -Wextra -Werror tests/touch_logic_test.cpp -o build/touch_logic_test
	./build/touch_logic_test
	$(CXX) -std=c++11 -Wall -Wextra -Werror -Wno-unused-function -fsanitize=address,undefined tests/asset_decode_test.cpp -o build/asset_decode_test
	python3 -m unittest discover -s tests -p 'test_*.py'

# Physically double-tap Reset first. Never use a 1200-baud reset for backup.
flash: build
	python3 tools/uf2.py flash build/Hallowing-touch.uf2 --volume "$(VOLUME)"

# Enter the bootloader physically first. Use when its disk will not mount.
# This SAMD21 BOSSA version auto-detects the application base at 8192 bytes.
flash-serial: build
	.arduino/data/packages/arduino/tools/bossac/1.7.0-arduino3/bossac --port=$(notdir $(PORT)) -U true -i -e -w -v build/eye/uncannyEyes.ino.bin -R

memory:
	python3 tools/memory.py

backup:
	python3 tools/uf2.py backup --volume "$(VOLUME)"

monitor:
	$(CLI) monitor --port "$(PORT)" --config baudrate=115200
