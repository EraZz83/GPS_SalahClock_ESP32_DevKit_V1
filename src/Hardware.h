#pragma once

#include "TM1637Display.h" // Display-Bibliothek
#include <time.h>          // Für die tm-Struktur

// --- Display Konfiguration ---
const int NUM_DISPLAYS = 6; 
const int CLK_PINS[NUM_DISPLAYS] = {15, 27, 25, 32, 4, 17}; 
const int DIO_PINS[NUM_DISPLAYS] = {2, 14, 26, 33, 16, 5}; 

// Array von Zeigern auf TM1637Display-Objekte
extern TM1637Display *displays[NUM_DISPLAYS]; 

// --- Globale Hardware-Zustände ---
extern int brightness;

// --- Funktionen Deklarationen ---

// Initialisiert alle Displays
void initDisplays(); 

// Aktualisiert ein einzelnes Display mit einem Zeitwert (HHMM)
void refreshDisplayValueBrightness(TM1637Display &display, int timeValue);