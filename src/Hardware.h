#pragma once

#include "TM1637Display.h" // Display-Bibliothek
#include <time.h>          // Für die tm-Struktur

// Initialisiert ein einzelnes Display
void initDisplays(TM1637Display &display);

// Aktualisiert ein einzelnes Display mit einem Zeitwert (HHMM)
void refreshDisplayValueBrightness(TM1637Display &display, int timeValue, int brightness);