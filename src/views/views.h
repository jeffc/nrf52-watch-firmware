/*
 * Views are the primary UI structure. A view is a bit like an "app" on a
 * smartphone. The active view's draw() method is called (by the system
 * controller object) when the screen updates, and when user events happen (like
 * button presses), the active view handles them.
 *
 * Inactive views still get their update method called, but they are not drawn.
 */
#ifndef _VIEWS_H_
#define _VIEWS_H_

class System;

class View {
  public:
    View(System* s);

    virtual void draw();
    virtual void update();
  
  protected:
    System* _sys;
};    


#endif
