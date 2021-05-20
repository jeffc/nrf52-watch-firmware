#pragma once

#ifdef EMBEDDED
#include <bluefruit.h>
#include "adafruit/BLEHidConsumer.h"
#endif

class BLE {

  public:
    BLE();

    void enable();
    void disable();

    bool isEnabled();

    void run();
    void test();

  private:
    //System* _sys;
    bool _is_enabled = false;

#ifdef EMBEDDED
BLEHidConsumer _hid;
    void startAdvertising();
#endif

};
