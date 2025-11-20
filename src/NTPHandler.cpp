#include "NTPHandler.h"

NTPHandler::NTPHandler(int gmtOffset_sec, int daylightOffset_sec, String  ntpServer)
{
    _gmtOffset_sec = gmtOffset_sec;
    _daylightOffset_sec = daylightOffset_sec;
    _ntpServer = ntpServer;
}

bool NTPHandler::initTimeSync()
{
    Serial.println("Starte NTP-Synchronisierung...");

    const char* ntpServerCStr = _ntpServer.c_str();

    configTime(_gmtOffset_sec, _daylightOffset_sec, ntpServerCStr);

    struct tm timeinfo_check;
    int attempts = 0;

    // Warte maximal 10 Sekunden auf die Zeit
    // ::getLocalTime ist die globale Funktion aus time.h
    while (!::getLocalTime(&timeinfo_check, 500) && attempts < 20)
    {
        Serial.print(".");
        delay(500);
        attempts++;
    }

    if (attempts >= 20)
    {
        Serial.println("\n❌ FEHLER: NTP-Synchronisierung fehlgeschlagen.");
        return false;
    }
    else
    {
        Serial.println("\n✅ NTP-Zeit erfolgreich abgerufen.");
        return true;
    }
}

// 3. Zeit abrufen
bool NTPHandler::getTime(tm &timeinfo)
{
    // ::getLocalTime ist die globale Funktion aus time.h
    if (::getLocalTime(&timeinfo, 100))
    {
        return true;
    }
    else
    {
        Serial.println("\n❌ FEHLER: Zeitabruf fehlgeschlagen.");
        return false;
    }
}