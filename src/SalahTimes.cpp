#include "SalahTimes.h"
#include <Arduino.h> 
#include <PrayerTimes.h> // Hinzugefügt, falls nicht vorhanden

SalahTimeCalculator::SalahTimeCalculator() {
    
    salahEngine = new PrayerTimes(LATITUDE, LONGITUDE, TIMEZONE);

    // 1. ZURÜCK ZUR MWL-METHODE: Diese verwendet den Shafi'i-Asr-Faktor, 
    // der Deiner gewünschten Kurve (14:36) am nächsten kommt.
    salahEngine->setCalculationMethod(MWL); 

    // 2. KORREKTUREN: Die Abweichungen zwischen MWL-Berechnung und Deiner Referenz (05:31, 12:11, 14:36)
    // werden als konstante Verschiebung über das Jahr hinweg angewendet.
    salahEngine->setAdjustments(-07, -14, -11, +1, +5, -9); 
    
    // Initialisiere die Zeiten auf Null
    for (int i = 0; i < 6; i++) {
        timesMinutes[i] = 0;
    }
}

void SalahTimeCalculator::updateTimes(const tm& timeinfo) {
    Serial.println("Updating Salah Times...");
    
    int currentMonth = timeinfo.tm_mon + 1; 
    int currentDay = timeinfo.tm_mday;
    int currentYear = timeinfo.tm_year + 1900;

    int fajrHour, fajrMinute;
    int sunriseHour, sunriseMinute;
    int dhuhrHour, dhuhrMinute;
    int asrHour, asrMinute;
    int maghribHour, maghribMinute;
    int ishaHour, ishaMinute;
    
    // KORREKTUR DER API: Aufruf der Methode calculate mit den 12 out-Parametern.
    salahEngine->calculate(
        currentDay, 
        currentMonth, 
        currentYear,
        fajrHour, fajrMinute,
        sunriseHour, sunriseMinute,
        dhuhrHour, dhuhrMinute,
        asrHour, asrMinute,
        maghribHour, maghribMinute,
        ishaHour, ishaMinute
    ); 

    // Konvertierung der Ergebnisse in Minuten seit Mitternacht und Speicherung
    timesMinutes[FAJR_IDX]    = fajrHour * 60 + fajrMinute;
    timesMinutes[SUNRISE_IDX] = sunriseHour * 60 + sunriseMinute;
    timesMinutes[DHUHR_IDX]   = dhuhrHour * 60 + dhuhrMinute;
    timesMinutes[ASR_IDX]     = asrHour * 60 + asrMinute;
    timesMinutes[MAGHRIB_IDX] = maghribHour * 60 + maghribMinute;
    timesMinutes[ISHA_IDX]    = ishaHour * 60 + ishaMinute;

    Serial.println("Salah Times updated.");
    
    // --- NEUE DEBUG-AUSGABE ---
    Serial.println("--- BERECHNETE ZEITEN (MWL/Shafi'i) ---");
    Serial.print("Fajr: "); Serial.println(convertMinutesToHHMM(timesMinutes[FAJR_IDX]));
    Serial.print("Sunrise: "); Serial.println(convertMinutesToHHMM(timesMinutes[SUNRISE_IDX]));
    Serial.print("Dhuhr: "); Serial.println(convertMinutesToHHMM(timesMinutes[DHUHR_IDX]));
    Serial.print("Asr: "); Serial.println(convertMinutesToHHMM(timesMinutes[ASR_IDX]));
    Serial.print("Maghrib: "); Serial.println(convertMinutesToHHMM(timesMinutes[MAGHRIB_IDX]));
    Serial.print("Isha: "); Serial.println(convertMinutesToHHMM(timesMinutes[ISHA_IDX]));
    Serial.println("---------------------------------------");
}

// Implementierung von convertMinutesToHHMM bleibt unverändert
int SalahTimeCalculator::convertMinutesToHHMM(int totalMinutes) {
    if (totalMinutes < 0 || totalMinutes >= 1440) return 0;
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    return hours * 100 + minutes;
}