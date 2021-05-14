#pragma once

#include "views/views.h"
#include <functional>
#include <string>
#include <utility>
#include <vector>

class Menu : public View {

  public:
    using View::View;
    void draw();
    void handleEvent(EVENT_T e);

    // function is called when the item is selected. returns true if the menu
    // should exit, false otherwise
    void addItem(std::string label, std::function<bool(void)> func);
    void addItem(std::function<std::string(void)> label_fn, std::function<bool(void)> func);

  private:
    std::vector<std::pair<std::function<std::string(void)>, std::function<bool(void)>>> _items;
    volatile int _pos = 0;
};
