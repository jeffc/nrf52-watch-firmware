#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_

#ifdef EMBEDDED
#include "FreeRTOS.h"
#include <task.h>
#endif

class Backlight {
  public:
    Backlight();

    void enableFor(unsigned ms);
    void off();

  private:
    TaskHandle_t handle;

    // duration is milliseconds cast to a void*
    static void backlightControlCallback(void* duration_ms);
};

#endif
