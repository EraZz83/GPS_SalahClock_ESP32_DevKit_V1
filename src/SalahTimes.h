#pragma once

#include <cmath> 
#include <ctime> 
#include <PrayerTimes.h> // Enthält CalculationMethod

// Konstanten für die Gebetszeiten-Berechnung
const double LATITUDE = 47.52; 
const double LONGITUDE = 9.38;
const int TIMEZONE = 1; // Als int für den Konstruktor

// Indices für die 6 angezeigten Zeiten (bleiben)
enum SalahIndex {
    FAJR_IDX       = 0, 
    SUNRISE_IDX    = 1, 
    DHUHR_IDX      = 2, 
    ASR_IDX        = 3, 
    MAGHRIB_IDX    = 4, 
    ISHA_IDX       = 5  
};

class SalahTimeCalculator {
public:
    int timesMinutes[6];

    // Konstruktor ist parameterlos (Methoden werden mit Setter in CPP gesetzt)
    SalahTimeCalculator(); 
    
    void updateTimes(const tm& timeinfo);

    static int convertMinutesToHHMM(int totalMinutes);
    
private:
    PrayerTimes *salahEngine; 
};