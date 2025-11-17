#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "ConfigManager.h"
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include "Hardware.h"
#include "SalahTimes.h"
#include <PrayerTimes.h>
#include "NTPHandler.h"

bool refreshDisplay = true;
int actTime;
int oldTimeMin;
int oldTimeSec;
struct tm timeinfo;

SalahTimeCalculator salahCalculator;

const char *ap_ssid = "salah_config_AP";
const char *ap_password = "123Polizei";
const char *sta_hostname = "salah";
const char *sta_ssid = "TurkishAirlines_";
const char *sta_password = "123Polizei";

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const int gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int brightnessOff = 64;
const int brightnessOn = 255;
const char *ntpServer = "pool.ntp.org";

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");

    startLittleFS();

    configManager.loadConfig();

    salahCalculator.initCalculator();
    initDisplays();

        if (connectWifi())
    {
        ntp
        configManager.initServer();
        configManager.startWebServer();
        setupOTA();
    }
    else
    {
        Serial.println("\n❌ WLAN-Verbindung fehlgeschlagen. Starte Konfigurationsmodus (AP)...");

        // Fallback auf AP
        WiFi.softAP(ap_ssid, ap_password);
        Serial.print("AP gestartet. IP: ");
        Serial.println(WiFi.softAPIP());

        // NTP im AP-Modus initialisieren (weniger wichtig, aber gut für den Zeitstempel)
        initTimeSync();
        configManager.initServer();
        configManager.startWebServer();
    }

    // 3. Erstberechnung (Erfordert erfolgreiche NTP-Synchronisierung, die nun gewartet wird)
    if (getLocalTime(&timeinfo))
    {
        salahCalculator.updateTimes(timeinfo);
    }
    else
    {
        Serial.println("❌ Konnte Zeit für Erstberechnung nicht abrufen. Zeiten bleiben 00:00.");
    }
}

void loop(void)
{
    ArduinoOTA.handle();
    printLocalTime();

    if (refreshDisplay)
    {
        for (int i = 0; i < NUM_DISPLAYS; i++)
        {
            int minutes = salahCalculator.timesMinutes[i];
            int timeToShow = SalahTimeCalculator::convertMinutesToHHMM(minutes);
            if refreshDisplayValueBrightness (*displays[i], timeToShow)
                ;
        }
        refreshDisplay = false;
    }
}