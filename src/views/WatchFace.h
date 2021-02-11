#ifndef _WATCHFACE_H_
#define _WATCHFACE_H_

#include "views/views.h"

class WatchFace : public View {

  public:
    using View::View;
    void draw();
};

#endif
