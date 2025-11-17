#include LittleFS.h

bool startLittleFS()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("❌ FEHLER: LittleFS konnte nicht initialisiert werden.");
        return false;
    }
    Serial.println("✅ LittleFS erfolgreich gestartet.");
    return true;
}