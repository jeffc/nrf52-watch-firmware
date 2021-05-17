#pragma once

#include "system/system.h"

class BLE {

  public:
    BLE(System* sys);

    void enable();
    void disable();

    bool isEnabled();

  private:
    System* _sys;
    bool _is_enabled = false;

};
