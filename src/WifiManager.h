#pragma once

#include <WiFi.h>

bool connectToWiFi(String ssid, String password, int maxTries);
bool startAP(String ap_ssid, String ap_password, IPAddress ip, IPAddress gateway, IPAddress subnet);