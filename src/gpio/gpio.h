#pragma once

#ifdef EMBEDDED
#include "FreeRTOS.h"
#include <task.h>
#endif

class GPIO {
  public:
    GPIO(int pin);

    void enableFor(unsigned ms);
    void on();
    void off();
    void toggle();
    bool isOn();

  private:
#ifdef EMBEDDED
    TaskHandle_t handle;
#endif
    int _pin;
    unsigned _on_for; // used for freertos callback
    bool _is_on;
};
