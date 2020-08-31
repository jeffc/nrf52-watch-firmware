# nrf52-watch

## Overview

This repo is, at present, a work-in-progress playground for designing and
building a smartwatch based on the nrf52840 microcontroller from Nordic.

## Parts

  * [RayTac MDBT50Q nRF52840 Module](https://www.raytac.com/product/ins.php?index_id=24)
    * Main brains. nRF52840 chipset chosen for BLE capability and low (&lt;1uA)
      sleep current.
    * This is the module used in the [Sparkfun Breakout](https://www.sparkfun.com/products/15025)
  * [Sharp LS018B7DH02 1.8" Memory LCD with FLExLight Frontlight](https://www.digikey.com/product-detail/en/flex-lighting/12616-06_T1/12616-06_T1-ND/9602610)
    * Always-on display.
    * Chosen for size and small power consumption.
    * The LS018B7DH02 isn't sold directly by Sharp, and no datasheet is
      currently available.
    * Unknowns are currently
      * Does the display require 3.3V or 5V power?
      * Does the display require 3.3V or 5V logic?
      * Does the display require a 1Hz or 60Hz square wave for biasing? 3.3V
        models tend to need 60Hz, but the 5V models need 1Hz
  * [M41T62Q6F RTC](https://www.digikey.com/product-detail/en/stmicroelectronics/M41T62Q6F/497-3907-1-ND/714738)
    * Realtime clock, for real timekeeping.
    * Chosen for small power consumption and footprint
    * This module can also provide the 1Hz or 64Hz square wave required by the
      memory LCD, which means fewer wakeups for the nRF52840
  * [MCP73831T Battery Charger IC](https://www.digikey.com/product-detail/en/microchip-technology/MCP73831T-2DCI-OT/MCP73831T-2DCI-OTCT-ND/1979804) 
    * Charges at 4.2V
  * USB-C port for power, charging, and data
    * I'm cheating here and using USB 2.0 Compatibility mode, so I don't get the
      benefits of USB-PD.

## Power

  * Project will be powered by a [500mAh LiPo](https://www.adafruit.com/product/1578),
    which has a nominal capacity of 500mAh at 3.7V.
    * Battery chosen has built-in overcurrent and undervoltage protection
      circuitry, meaning that I don't have to design any.
  * Projected lifetime is &gt;2mo, though it remains to be seen if this is
    possible

## Manufacture and Assembly

  * Prototypes of the case and mechanical components will be 3D printed.
  * Once I have a design that I like, I'll send the files to
    [Shapeways](http://shapeways.com) or [ExOne](http://exone.com) to be 3D
    Printed in steel.

## Software

  * To conserve power, everything will be interrupts-driven. 
  * A decent amount of build hackery was needed to make the native version build
    with the dummy adafruit_gfx library

## Credits / Inspiration

  * This project uses open-source code from the following projects. Licenses are
    included alongside code where applicable.
    * [Adafruit](https://github.com/adafruit/Adafruit-GFX-Library)
    * [vhervieux on github](https://github.com/croutor/Adafruit_GFX_dummy_display)
    * [jeraymond on github](https://github.com/jeraymond/pio-sparkfun_pro_nrf52840_mini-setup)
  * This project took inspiration from these projects:
    * [Chronio](https://hackaday.io/project/12876-chronio)
    * [Samson March's Smartwatch](https://imgur.com/a/FSBwD3g)
