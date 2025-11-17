#pragma once

#include <time.h> // Für configTime

// Deklaration der Hauptfunktion für NTP und mDNS
void setupNetworkServices(const char *hostname, const char *ntpServer, int gmtOffset, int daylightOffset);