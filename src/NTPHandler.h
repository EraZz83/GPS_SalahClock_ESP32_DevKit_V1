#pragma once

class NTPHandler
{
public:
    bool initTimeSync(int gmtOffset_sec, int daylightOffset_sec, char ntpServer);
    bool getLocalTime(tm &timeinfo);

private:
    int gmtOffset_sec;
    int daylightOffset_sec;
    char ntpServer;
};