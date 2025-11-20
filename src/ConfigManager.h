#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

struct ClockConfig
{
    float latitude = 47.52777;
    float longitude = 9.37074;
    int timeZone = 1;
    String sta_hostname = "salah";
    String sta_ssid = "TurkishAirlines_";
    String sta_password = "123Polizei";
    String ap_ssid = "Salah_Fallback_AP";
    String ap_password = "123Polizei";
};

class ConfigManager
{
public:
    ConfigManager();
    void loadConfig();
    void saveConfig();
    void initServer();
    void startWebServer();

    ClockConfig config;
    AsyncWebServer *server = nullptr;
};

extern ConfigManager configManager;