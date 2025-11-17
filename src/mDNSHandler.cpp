#include "mDNSHandler.h"
#include <ESPmDNS.h>
#include <WiFi.h>

void setupNetworkServices(const char *hostname, const char *ntpServer, int gmtOffset, int daylightOffset)
{
    if (!MDNS.begin(hostname))
    {
        Serial.println("❌ mDNS konnte nicht gestartet werden.");
        return;
    }

    MDNS.addService("http", "tcp", 80);
    Serial.println("✅ mDNS gestartet und Hostname ist: " + String(hostname) + ".local");
}