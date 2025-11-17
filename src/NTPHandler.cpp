#include <time.h>

class NTPHandler
{
    initTimeSync(int gmtOffset_sec, int daylightOffset_sec, char ntpServer)
    {
        Serial.println("Starte NTP-Synchronisierung...");

        // Init und Konfiguration der Zeitserver
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        struct tm timeinfo_check;
        int attempts = 0;

        // Warte maximal 10 Sekunden auf die Zeit
        while (!getLocalTime(&timeinfo_check, 500) && attempts < 20)
        {
            Serial.print(".");
            delay(500);
            attempts++;
        }

        if (attempts >= 20)
        {
            Serial.println("\n❌ FEHLER: NTP-Synchronisierung fehlgeschlagen.");
            return false
        }
        else
        {
            Serial.println("\n✅ NTP-Zeit erfolgreich abgerufen.");
            return true
        }
    }

    bool NTPHandler::getLocalTime(tm &timeinfo)
    {
        if (getLocalTime(timeinfo, 100))
        {
            Serial.println("\n✅ NTP-Zeit erfolgreich abgerufen.");
            return true;
        }

        else
        {
            Serial.println("\n❌ FEHLER: NTP-Synchronisierung fehlgeschlagen.");
            return false
        }
    }
}
