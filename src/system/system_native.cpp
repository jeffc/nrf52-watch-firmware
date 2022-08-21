#ifdef NATIVE

#include "pins.h"
#include "system.h"
#include <Adafruit_GFX_dummy_display.h>
#include <map>
#include <mutex>
#include <set>
#include <stdint.h>
#include <stdio.h>

std::mutex System::runlock;
std::set<int> System::buttonsPressed;

typedef void (*callbackFunctionType)(void); 
std::map<std::pair<int,int>,callbackFunctionType> callbackMap;


System::System() {
  commonSetup();
}

void System::registerIRQ(int pinnum, void (*fn)(), int mode) {
  std::pair<int,int> mapkey = {pinnum, mode};
  if (callbackMap.count(mapkey) > 0) {
    printf("Tried to register duplicate IRQ %d (%d)\n", pinnum, mode);
  } else {
    callbackMap[mapkey] = fn;
    printf("Registered IRQ %d (%d)\n", pinnum, mode);
  }
}

void System::fireIRQ(int pin, int mode) {
  std::pair<int,int> mapkey = {pin, mode};
  auto it = callbackMap.find(mapkey);
  if (it != callbackMap.end()) {
    runlock.lock();
    it->second();
    runlock.unlock();
  }
}

void System::feedWatchdog() {}

bool System::getButtonPressed(int pin) {
  return (System::buttonsPressed.count(pin) > 0);
}

void System::enableSerial() {
  printf("Enabled Serial");
}

void System::disableSerial() {
  printf("Disabled Serial");
}

void System::reboot() {
  printf("Rebooting");
  exit(0);
}

#endif
