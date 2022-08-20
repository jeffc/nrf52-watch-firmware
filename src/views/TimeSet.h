#ifndef _TIMESET_H_
#define _TIMESET_H_

#include "views/views.h"

class TimeSet : public View {

  public:
    using View::View;
    void draw();
    void handleEvent(EVENT_T e);
};

#endif
