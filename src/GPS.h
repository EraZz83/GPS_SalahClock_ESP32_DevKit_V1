#include "TinyGPSPlus.h"
using namespace std;

unsigned long oldRefresh = 0;
bool refreshTick;

refreshTick = false;

if (((millis() - oldRefresh) >= 5000) || (oldRefresh == 0))
{
    oldRefresh = millis();
    refreshTick = true;
}

bool checkForconnection()
{
  if (refreshTick)
  {
    return (GPS.satellites.value() > 0)
  }
}