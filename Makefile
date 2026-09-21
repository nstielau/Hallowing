CLI := .tools/arduino-cli/arduino-cli --config-file .arduino/arduino-cli.yaml
FQBN := adafruit:samd:adafruit_hallowing
PORT ?= /dev/cu.usbmodem1101
VOLUME ?= /Volumes/HALLOWBOOT

.PHONY: setup build test flash backup monitor
setup:
	bash tools/setup.sh

build:
	$(CLI) compile --fqbn $(FQBN) --build-path build/eye firmware/uncannyEyes
	python3 tools/uf2.py pack build/eye/uncannyEyes.ino.bin build/Hallowing-touch.uf2

test:
	mkdir -p build
	$(CXX) -std=c++11 -Wall -Wextra -Werror tests/touch_logic_test.cpp -o build/touch_logic_test
	./build/touch_logic_test
	python3 -m unittest discover -s tests -p 'test_*.py'

# Physically double-tap Reset first. Never use a 1200-baud reset for backup.
flash: build
	python3 tools/uf2.py flash build/Hallowing-touch.uf2 --volume "$(VOLUME)"

backup:
	python3 tools/uf2.py backup --volume "$(VOLUME)"

monitor:
	$(CLI) monitor --port "$(PORT)" --config baudrate=115200
