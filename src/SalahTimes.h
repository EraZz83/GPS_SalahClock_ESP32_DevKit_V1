#pragma once

#include <cmath>
#include <ctime>
#include <PrayerTimes.h>
#include "ConfigManager.h" // Muss inkludiert werden, um auf configManager zuzugreifen

// Indices für die 6 angezeigten Zeiten
enum SalahIndex
{
    FAJR_IDX = 0,
    SUNRISE_IDX = 1,
    DHUHR_IDX = 2,
    ASR_IDX = 3,
    MAGHRIB_IDX = 4,
    ISHA_IDX = 5
};

class SalahTimeCalculator
{
public:
    int timesMinutes[6];

    // Konstruktor ist leer, da die Instanziierung verschoben wurde
    SalahTimeCalculator();

    // NEU: Initialisiert die PrayerTimes-Instanz mit den aktuellen Geo-Daten.
    void initCalculator();
    bool checkTimechange(const tm &timeinfo);
    void updateTimes(const tm &timeinfo);

    static int convertMinutesToHHMM(int totalMinutes);

private:
    PrayerTimes *salahEngine;
};