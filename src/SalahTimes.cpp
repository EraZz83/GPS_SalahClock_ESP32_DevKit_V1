#include "SalahTimes.h" // Enthält die Klassendefinition und die Indizes!
#include <Arduino.h>
#include <PrayerTimes.h>
#include "ConfigManager.h"

// --- Implementierung des Konstruktors ---

SalahTimeCalculator::SalahTimeCalculator()
{

    // Wichtig: Die Konstanten LATITUDE etc. existieren nicht mehr, wir greifen auf configManager zu
    salahEngine = new PrayerTimes(
        configManager.config.latitude,
        configManager.config.longitude,
        configManager.config.timeZone);

    salahEngine = nullptr; // Sicherstellen, dass Pointer null ist

    for (int i = 0; i < 6; i++)
    {
        timesMinutes[i] = 0;
    }
}

void SalahTimeCalculator::initCalculator()
{
    // 1. Instanziierung mit dynamischen Werten
    if (salahEngine != nullptr)
    {
        delete salahEngine; // Alte Instanz löschen
    }

    salahEngine = new PrayerTimes(
        configManager.config.latitude,
        configManager.config.longitude,
        configManager.config.timeZone);

    // 2. Berechnungsmethode und Adjustments setzen
    salahEngine->setCalculationMethod(MWL);
    salahEngine->setAdjustments(-7, -14, -11, 1, 5, -9);
}

bool SalahTimeCalculator::checkTimechange(const tm &timeinfo)
{
    if ((timeinfo.tm_hour == 0) && (timeinfo.tm_min == 0) && (timeinfo.tm_sec == 1))
    {
        return true;
    }
    return false;
}

// --- Implementierung von updateTimes ---

void SalahTimeCalculator::updateTimes(const tm &timeinfo)
{
    if (salahEngine == nullptr)
    {
        initCalculator(); // Sicherstellen, dass die Berechnung möglich ist
    }

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
        ishaHour, ishaMinute);

    // Konvertierung der Ergebnisse in Minuten seit Mitternacht und Speicherung
    // WICHTIG: Die Indizes sind jetzt durch #include "SalahTimes.h" sichtbar!
    timesMinutes[FAJR_IDX] = fajrHour * 60 + fajrMinute;
    timesMinutes[SUNRISE_IDX] = sunriseHour * 60 + sunriseMinute;
    timesMinutes[DHUHR_IDX] = dhuhrHour * 60 + dhuhrMinute;
    timesMinutes[ASR_IDX] = asrHour * 60 + asrMinute;
    timesMinutes[MAGHRIB_IDX] = maghribHour * 60 + maghribMinute;
    timesMinutes[ISHA_IDX] = ishaHour * 60 + ishaMinute;

    Serial.println("Salah Times updated.");

    // --- NEUE DEBUG-AUSGABE ---
    Serial.println("--- BERECHNETE ZEITEN (MWL/Shafi'i) ---");
    // WICHTIG: convertMinutesToHHMM ist static, daher SalahTimeCalculator::
    Serial.print("Fajr: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[FAJR_IDX]));
    Serial.print("Sunrise: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[SUNRISE_IDX]));
    Serial.print("Dhuhr: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[DHUHR_IDX]));
    Serial.print("Asr: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[ASR_IDX]));
    Serial.print("Maghrib: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[MAGHRIB_IDX]));
    Serial.print("Isha: ");
    Serial.println(SalahTimeCalculator::convertMinutesToHHMM(timesMinutes[ISHA_IDX]));
    Serial.println("---------------------------------------");
}

// --- Implementierung von convertMinutesToHHMM ---

int SalahTimeCalculator::convertMinutesToHHMM(int totalMinutes)
{
    if (totalMinutes < 0 || totalMinutes >= 1440)
        return 0;
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    return hours * 100 + minutes;
}