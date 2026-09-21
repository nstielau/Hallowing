# Adafruit Uncanny Eyes

`firmware/uncannyEyes/uncannyEyes.ino`, `config.h`, `user.cpp`, and
`graphics/defaultEye.h` come from [Adafruit Uncanny Eyes](https://github.com/adafruit/Uncanny_Eyes)
at commit `d2103e84aa33da9f6924885ebc06d880af8deeff`.

The upstream sketch identifies its license as MIT and its author as Phil Burgess
/ Paint Your Dragon for Adafruit Industries. The original attribution is retained.
The original human artwork remains unchanged in `graphics/defaultEye.h` and is
stored losslessly in the generated `graphics/eye_assets.h`. `assets/upstream/`
contains gzip-compressed, unmodified `dragonEye.h` and `goatEye.h` from the same
commit, with source checksums in `source.json`. Their iris textures and pupil maps
are resampled for this three-style build by `tools/build_assets.py`.
The sketch and configuration integrate capacitive touch, style selection, and
scanline decompression with the existing movement and blink state machines.

MIT License

Copyright (c) Phil Burgess / Paint Your Dragon for Adafruit Industries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
