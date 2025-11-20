#include <Arduino.h>
#include "ConfigManager.h"
#include <OTAHandler.h>
#include <ESPmDNS.h>
#include "Hardware.h"
#include "SalahTimes.h"
#include "NTPHandler.h"
#include "LittleFSHandler.h"
#include <WiFiManager.h>

bool refreshDisplay = true;
int brightness = 7;
struct tm timeinfo;

SalahTimeCalculator salahCalculator;

const int NUM_DISPLAYS = 6;
const int CLK_PINS[NUM_DISPLAYS] = {15, 27, 25, 32, 4, 17};
const int DIO_PINS[NUM_DISPLAYS] = {2, 14, 26, 33, 16, 5};
TM1637Display *displays[NUM_DISPLAYS];

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const int gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int brightnessOff = 64;
const int brightnessOn = 255;
const char *ntpServer = "pool.ntp.org";

NTPHandler ntpHandler(gmtOffset_sec, daylightOffset_sec, ntpServer);

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");

    for (int i = 0; i < NUM_DISPLAYS; i++)
    {
        displays[i] = new TM1637Display(CLK_PINS[i], DIO_PINS[i]);
        initDisplays(*displays[i]);
    }

    initLittleFS();

    configManager.loadConfig();
    salahCalculator.initCalculator();

    if (connectToWiFi(configManager.config.sta_ssid, configManager.config.sta_password, 5))
    {
        ntpHandler.initTimeSync();
        configManager.initServer();
        configManager.startWebServer();
        setupOTA(configManager.config.sta_hostname);
    }
    else
    {
        Serial.println("\n❌ WLAN-Verbindung fehlgeschlagen. Starte Konfigurationsmodus (AP)...");

        startAP(configManager.config.ap_ssid, configManager.config.ap_password, local_IP, gateway, subnet);
        Serial.print("AP gestartet. IP: ");
        Serial.println(WiFi.softAPIP());

        configManager.initServer();
        configManager.startWebServer();
    }

    if (ntpHandler.getTime(timeinfo))
    {
        salahCalculator.updateTimes(timeinfo);
    }
    else
    {
        Serial.println("❌ Konnte Zeit für Erstberechnung nicht abrufen. Zeiten bleiben 00:00.");
    }

    for (int i = 0; i < NUM_DISPLAYS; i++)
    {
        int minutes = salahCalculator.timesMinutes[i];
        int timeToShow = SalahTimeCalculator::convertMinutesToHHMM(minutes);
        refreshDisplayValueBrightness(*displays[i], timeToShow, brightness);
    }
}

void loop(void)
{
    ArduinoOTA.handle();

    if (salahCalculator.checkTimechange(timeinfo))
    {
        salahCalculator.updateTimes(timeinfo);
        refreshDisplay = true;
    }

    if (refreshDisplay)
    {
        for (int i = 0; i < NUM_DISPLAYS; i++)
        {
            int minutes = salahCalculator.timesMinutes[i];
            int timeToShow = SalahTimeCalculator::convertMinutesToHHMM(minutes);
            refreshDisplayValueBrightness(*displays[i], timeToShow, brightness);
        }
        refreshDisplay = false;
    }
}