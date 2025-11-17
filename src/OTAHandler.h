#pragma once
#define DEBUG_

#include <ArduinoOTA.h>

void setupOTA(const char* hostname);
void handleOTA();