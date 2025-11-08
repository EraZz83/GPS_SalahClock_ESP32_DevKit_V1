#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "Hardware.h"       
#include "SalahTimes.h"     
#include <PrayerTimes.h>    

// --- Globale Variablen Definitionen ---
bool refreshDisplay = true; 
int actTime; 
int oldTimeMin;
int oldTimeSec;
struct tm timeinfo;

// Instanziierung ohne Parameter. Die Konfiguration erfolgt in SalahTimes.cpp.
SalahTimeCalculator salahCalculator; 

// --- WiFi & NTP Konfiguration ---
const char* ssid = "TurkishAirlines_";
const char* password = "123Polizei";
const int gmtOffset_sec = 3600;         
const int daylightOffset_sec = 3600;    
const char* ntpServer = "pool.ntp.org";

// --- Zeit- und Salah-Logik ---

void printLocalTime()
{
  if(!getLocalTime(&timeinfo, 100))
  {
    return;
  }
  
  actTime = timeinfo.tm_hour * 100 + timeinfo.tm_min;

  if ((oldTimeSec != timeinfo.tm_sec))
  {
    oldTimeMin = timeinfo.tm_min;
    oldTimeSec = timeinfo.tm_sec;
    refreshDisplay = true; 
    Serial.printf("Time: %02d:%02d:%02d - Refreshing displays.\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 1 && timeinfo.tm_sec == 0) {
        salahCalculator.updateTimes(timeinfo);
    }
  }
}

void setup(void) {
    Serial.begin(115200);
    Serial.println("Booting");
    
    initDisplays(); 

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA.begin();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    if (!MDNS.begin("salah")) 
    {
        Serial.println("Error setting up mDNS.");
    }
    MDNS.addService("http", "tcp", 80);

    if (getLocalTime(&timeinfo)) {
        salahCalculator.updateTimes(timeinfo);
    } else {
        Serial.println("Could not get time for initial Salah calculation.");
    }
}


void loop(void) {
    ArduinoOTA.handle();
    printLocalTime();
    
    if (refreshDisplay) {
        for (int i = 0; i < NUM_DISPLAYS; i++) {
            int minutes = salahCalculator.timesMinutes[i];
            
            int timeToShow = SalahTimeCalculator::convertMinutesToHHMM(minutes);
            
            refreshDisplayValueBrightness(*displays[i], timeToShow); 
        }
        refreshDisplay = false; 
    }
}