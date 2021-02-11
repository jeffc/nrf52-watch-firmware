#include <system/system.h>
#include <views/views.h>
#include <views/WatchFace.h>


void System::initBaseView() {
  _active_view = new WatchFace(this);
}

View* System::getActiveView() {
  return _active_view;
}
