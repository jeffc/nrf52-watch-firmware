#include "views/PopupView.h"
#include "system/system.h"
#include <fonts/Dustfine72pt7b.h>

void PopupView::draw() {
  Graphics *gfx = _sys->getGraphics();

  gfx->clearBuffer();

  gfx->setTextColor(0);
  gfx->setFont(&Dustfine72pt7b);
  gfx->setCursor(45, 120);
  gfx->setTextColor(0);
  gfx->printf(":)");
}

void PopupView::handleEvent(EVENT_T e) {
  if (e.type == BUTTON_PRESS && e.button == BUTTON_MIDDLE) {
    exit();
  }
}
