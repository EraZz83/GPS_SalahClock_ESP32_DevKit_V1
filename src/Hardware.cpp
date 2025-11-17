#include "Hardware.h"
#include <Arduino.h> // Notwendig für Serial.println, pinMode etc. (falls genutzt)

// --- Globale Variablen Definitionen ---
// Definition des Display-Arrays, das in Hardware.h deklariert wurde
TM1637Display *displays[NUM_DISPLAYS]; 

// Definition der globalen Helligkeitsvariable
int brightness = 1;

// --- Hardware Implementierungen ---

/**
 * Initialisiert alle TM1637Display-Objekte mithilfe der Pin-Arrays.
 */
void initDisplays() {
    for (int i = 0; i < NUM_DISPLAYS; i++) {
        // Erstellt das Objekt dynamisch und speichert den Zeiger
        displays[i] = new TM1637Display(CLK_PINS[i], DIO_PINS[i]);
        
        // Konfiguriert das Display
        displays[i]->clear();
        displays[i]->setBrightness(brightness, true);
    }
}

/**
 * Aktualisiert das übergebene Display mit der angegebenen Zeit im HHMM-Format
 * und wendet die aktuelle Helligkeit an.
 */
void refreshDisplayValueBrightness(TM1637Display &display, int timeValue, int brightness)
{
    // Die refreshDisplay-Variable wird in main.cpp verwaltet, daher 
    // muss diese Funktion davon entkoppelt sein oder das Flag übergeben bekommen.
    // Da wir das Flag in loop() prüfen, gehen wir davon aus, dass hier aktualisiert werden soll.

    // Helligkeits-Logik
    if (brightness > 0)
    {
      display.setBrightness(brightness, true);
    }
    else
    {
      display.setBrightness(brightness, false);
    }
    
    // Anzeige des Zeitwerts (HHMM) mit Doppelpunkt
    // 0b01000000 setzt den Doppelpunkt
    display.showNumberDecEx(timeValue, 0b01000000, true);
}