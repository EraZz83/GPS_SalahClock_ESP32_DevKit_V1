#pragma once
#include <time.h>
#include <Arduino.h>

class NTPHandler
{
public:
    NTPHandler(int gmtOffset_sec, int daylightOffset_sec, String ntpServer);

    bool initTimeSync();
    bool getTime(tm &timeinfo);

private:
    int _gmtOffset_sec;
    int _daylightOffset_sec;
    String _ntpServer;
};