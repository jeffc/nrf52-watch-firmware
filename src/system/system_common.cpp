#include <system/system.h>
#include <views/views.h>
#include <views/WatchFace.h>


// force-clears the view stack
void System::initBaseView() {
  while (_view_stack.size() > 0) {
    View* v = _view_stack.top();
    _view_stack.pop();
    delete v;
  }
  _view_stack.push(new WatchFace(this));
    
}

View* System::getActiveView() {
  while (_view_stack.size() > 1 &&_view_stack.top()->isDead()) {
    View* v = _view_stack.top();
    delete v;
    _view_stack.pop();
  }
  return _view_stack.top();
}

void System::switchToNewView(View* v) {
  _view_stack.push(v);
}

