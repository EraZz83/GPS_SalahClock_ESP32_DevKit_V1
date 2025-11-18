#include "Hardware.h"
#include <Arduino.h>

void initDisplays(TM1637Display &display)
{
  display.clear();
  display.setBrightness(7, true);
}

void refreshDisplayValueBrightness(TM1637Display &display, int timeValue, int brightness)
{
  if (brightness > 0)
  {
    display.setBrightness(brightness, true);
  }
  else
  {
    display.setBrightness(0, false);
  }

  // Anzeige des Zeitwerts (HHMM) mit Doppelpunkt
  // 0b01000000 setzt den Doppelpunkt
  display.showNumberDecEx(timeValue, 0b01000000, true);
}