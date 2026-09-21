#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."

# Dependencies stay inside this checkout.
CLI=.tools/arduino-cli/arduino-cli
VERSION=1.5.1
if [ ! -x "$CLI" ]; then
  case "$(uname -s)-$(uname -m)" in
    Darwin-arm64) PLATFORM=macOS_ARM64 ;;
    Darwin-x86_64) PLATFORM=macOS_64bit ;;
    Linux-x86_64) PLATFORM=Linux_64bit ;;
    Linux-aarch64) PLATFORM=Linux_ARM64 ;;
    *) echo 'Install Arduino CLI 1.5.1 at .tools/arduino-cli/arduino-cli for this platform.' >&2; exit 1 ;;
  esac
  mkdir -p .tools/arduino-cli
  ARCHIVE="arduino-cli_${VERSION}_${PLATFORM}.tar.gz"
  URL="https://github.com/arduino/arduino-cli/releases/download/v${VERSION}"
  curl --http1.1 -fsSL --retry 3 "$URL/$ARCHIVE" -o ".tools/arduino-cli/$ARCHIVE"
  curl --http1.1 -fsSL --retry 3 "$URL/${VERSION}-checksums.txt" -o .tools/arduino-cli/checksums.txt
  python3 - "$ARCHIVE" <<'PY'
import hashlib
from pathlib import Path
import sys
root = Path('.tools/arduino-cli')
name = sys.argv[1]
expected = next(line.split()[0] for line in (root / 'checksums.txt').read_text().splitlines() if line.split()[-1] == name)
if hashlib.sha256((root / name).read_bytes()).hexdigest() != expected:
    raise SystemExit('Arduino CLI archive checksum mismatch')
PY
  tar -xzf ".tools/arduino-cli/$ARCHIVE" -C .tools/arduino-cli
fi

mkdir -p .arduino
CONFIG=.arduino/arduino-cli.yaml
if [ ! -f "$CONFIG" ]; then "$CLI" config init --dest-dir .arduino; fi
for name in data downloads user; do
  "$CLI" --config-file "$CONFIG" config set "directories.$name" "$PWD/.arduino/$name"
done
"$CLI" --config-file "$CONFIG" config set board_manager.additional_urls https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
# Avoid intermittent HTTP/2 download errors with the tool archives.
export GODEBUG=http2client=0
"$CLI" --config-file "$CONFIG" core update-index
"$CLI" --config-file "$CONFIG" core install adafruit:samd@1.7.17
"$CLI" --config-file "$CONFIG" lib install \
  'Adafruit FreeTouch Library@1.1.3' \
  'Adafruit GFX Library@1.12.6' \
  'Adafruit ST7735 and ST7789 Library@1.11.0' \
  'Adafruit Zero DMA Library@1.1.4' \
  'Adafruit BusIO@1.17.4'
