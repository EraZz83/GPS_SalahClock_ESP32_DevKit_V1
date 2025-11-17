#pragma once
#define DEBUG_

bool connectToWiFi(const char *ssid, const char *password, int maxTries);
bool startAP(const char *ap_ssid, const char *ap_password, IPAddress ip, IPAddress gateway, IPAddress subnet);