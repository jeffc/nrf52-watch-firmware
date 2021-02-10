This is a modified version of the adafruit sharp memory lcd library.
The modification is for the LS018B7DH02 1.8" memory LCD (and possibly others),
which require 9 bits to address their lines. The original readme is included as
README_ADAFRUIT.txt. The code can be found at
https://github.com/adafruit/Adafruit_SHARP_Memory_Display/

This library was further modified to use hardware SPI instead of bitbanged
pseudo-spi. This is approximately 4x faster (39ms instead of ~170ms for a full
update).

Why does it have to be in lib/ with header guards instead of lib_nrf ? It just
does. PlatformIO's dependency graph outsmarted me otherwise.
