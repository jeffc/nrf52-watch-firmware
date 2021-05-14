#pragma once

#ifdef EMBEDDED
#include "FreeRTOS.h"
#include <task.h>
#endif

class GPIO {
  public:
    GPIO(int pin);

    void enableFor(unsigned ms);
    void off();

  private:
#ifdef EMBEDDED
    TaskHandle_t handle;
#endif

    // duration is milliseconds cast to a void*
    static void backlightControlCallback(void* duration_ms);
    int _pin;
};
