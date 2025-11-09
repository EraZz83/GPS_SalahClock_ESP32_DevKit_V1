#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "ConfigManager.h" 
#include <ArduinoOTA.h>
#include <ESPmDNS.h> // mDNS Logic ist jetzt hier
#include "Hardware.h"       
#include "SalahTimes.h"     
#include <PrayerTimes.h>    
#include <time.h> // Für getLocalTime

// --- GLOBALE VARIABLEN ---
bool refreshDisplay = true; 
int actTime; 
int oldTimeMin;
int oldTimeSec;
struct tm timeinfo;

SalahTimeCalculator salahCalculator; 

// --- WiFi & NTP Konfiguration ---
const char* ap_ssid = "salah_config_AP"; 
const char* ap_password = "salahpassword"; 
const char* sta_hostname = "salah";     
const char* sta_ssid = "TurkishAirlines_";
const char* sta_password = "123Polizei";
const int gmtOffset_sec = 3600;         
const int daylightOffset_sec = 3600;    
const char* ntpServer = "pool.ntp.org";

// --- DEKLARATIONEN ---
void printLocalTime(); 
void setupOTA();
void initTimeSync(); // Deklaration für NTP/mDNS


// --- FUNKTION: NTP Synchronisierung und mDNS Start ---
void initTimeSync() {
    Serial.println("Starte NTP-Synchronisierung...");
    
    // Init und Konfiguration der Zeitserver
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // WICHTIG: Warte auf erfolgreichen Zeitabruf
    struct tm timeinfo_check;
    int attempts = 0;
    
    // Warte maximal 10 Sekunden auf die Zeit
    while (!getLocalTime(&timeinfo_check, 500) && attempts < 20) {
        Serial.print(".");
        delay(500);
        attempts++;
    }
    
    if (attempts >= 20) {
        Serial.println("\n❌ FEHLER: NTP-Synchronisierung fehlgeschlagen.");
    } else {
        Serial.println("\n✅ NTP-Zeit erfolgreich abgerufen.");
    }
    
    // mDNS Initialisierung
    if (!MDNS.begin(sta_hostname)) {
        Serial.println("❌ mDNS konnte nicht gestartet werden.");
    }
    MDNS.addService("http", "tcp", 80);
}


// --- FUNKTION: OTA-Setup ---
void setupOTA() {
    ArduinoOTA.setHostname(sta_hostname);
    ArduinoOTA.begin();
    Serial.println("✅ OTA-Dienst gestartet.");
}


// --- FUNKTION: printLocalTime (Wiederhergestellt) ---
void printLocalTime()
{
    if(!getLocalTime(&timeinfo, 100)) return; // Checkt die Zeit, bricht bei Fehler ab
    
    // Aktuelle Zeit in HHMM
    actTime = timeinfo.tm_hour * 100 + timeinfo.tm_min;

    // Prüfen, ob sich die Sekunde geändert hat
    if ((oldTimeSec != timeinfo.tm_sec))
    {
        oldTimeMin = timeinfo.tm_sec;
        oldTimeSec = timeinfo.tm_sec;
        refreshDisplay = true; 
        Serial.printf("Time: %02d:%02d:%02d - Refreshing displays.\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

        // Gebetszeiten einmal täglich aktualisieren
        if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 1 && timeinfo.tm_sec == 0) {
            salahCalculator.updateTimes(timeinfo);
        }
    }
}


void setup() {
    Serial.begin(115200);
    Serial.println("Booting");
    
    // 1. LITTLEFS Start
    if (!LittleFS.begin(true)) {
        Serial.println("❌ FEHLER: LittleFS konnte nicht initialisiert werden.");
    }
    
    configManager.loadConfig(); 
    
    salahCalculator.initCalculator();
    initDisplays(); 

    // 2. WLAN starten
    WiFi.mode(WIFI_STA);
    WiFi.begin(sta_ssid, sta_password);
    
    Serial.print("Verbinde mit WLAN...");
    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 30) {
        delay(500);
        Serial.print(".");
        i++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Verbunden!");
        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.localIP());
        
        // --- STATION MODE DIENSTE ---
        initTimeSync(); // Korrigiert: Wartet jetzt auf NTP-Zeit
        configManager.initServer(); 
        configManager.startWebServer(); 
        setupOTA(); 

    } else {
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
    if (getLocalTime(&timeinfo)) {
        salahCalculator.updateTimes(timeinfo);
    } else {
        Serial.println("❌ Konnte Zeit für Erstberechnung nicht abrufen. Zeiten bleiben 00:00.");
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