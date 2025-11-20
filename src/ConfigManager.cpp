#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

// Globale Instanz
ConfigManager configManager;

ConfigManager::ConfigManager()
{
    ;
}

void ConfigManager::initServer()
{
    if (server == nullptr)
    {
        server = new AsyncWebServer(80);
        Serial.println("Async WebServer instanziiert.");
    }
}
void ConfigManager::loadConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println("WARNUNG: config.json nicht gefunden, verwende Standardwerte.");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, configFile);

    if (error)
    {
        Serial.print("Fehler beim Lesen der JSON-Datei: ");
        Serial.println(error.c_str());
        return;
    }

    config.latitude = doc["lat"] | config.latitude;
    config.longitude = doc["lon"] | config.longitude;
    config.timeZone = doc["tz"] | config.timeZone;

    Serial.println("Konfiguration erfolgreich geladen.");
    Serial.printf("LAT: %.4f, LON: %.4f, TZ: %d\n", config.latitude, config.longitude, config.timeZone);
    configFile.close();
}

void ConfigManager::saveConfig()
{
    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("FEHLER: Konnte config.json nicht zum Schreiben öffnen.");
        return;
    }

    StaticJsonDocument<256> doc;
    doc["lat"] = config.latitude;
    doc["lon"] = config.longitude;
    doc["tz"] = config.timeZone;
    doc["stassid"] = config.sta_ssid;
    doc["stapw"] = config.sta_password;
    doc["stahn"] = config.sta_hostname;
    doc["apssid"] = config.ap_ssid;
    doc["appw"] = config.ap_password;

    if (serializeJson(doc, configFile) == 0)
    {
        Serial.println("FEHLER beim Schreiben der Konfigurationsdatei!");
    }
    else
    {
        Serial.println("Konfiguration erfolgreich gespeichert.");
    }
    configFile.close();
}

// Hilfsfunktion zum Laden einer Datei aus LittleFS
String loadTemplateFile(const char *path)
{
    File file = LittleFS.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.printf("❌ FEHLER: Datei %s nicht gefunden.\n", path);
        return "<h1>Konfigurationsdatei nicht gefunden!</h1><p>Bitte FS hochladen.</p>";
    }
    String content = file.readString();
    file.close();
    return content;
}

// ... (loadConfig und saveConfig Implementierung folgt hier) ...
// (Länge des Codes wegen Platzersparnis ausgelassen, verwende Deinen bestehenden Code)
// ...

void ConfigManager::startWebServer()
{

    if (server == nullptr)
    {
        Serial.println("FEHLER: Webserver nicht instanziiert. Rufen Sie initServer() in setup() auf.");
        return;
    }

    // 1. ROOT-ROUTE (salah.local): Zeigt das dynamische Konfigurationsformular
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
               {
        // Lade das HTML-Template (config.html) aus dem Dateisystem
        String html = loadTemplateFile("/config.html");
        
        // Platzhalter ersetzen
        html.replace("%LAT%", String(config.latitude, 6));
        html.replace("%LON%", String(config.longitude, 6));
        html.replace("%TZ%", String(config.timeZone));
        html.replace("%STAHOSTNAME%", String(config.sta_hostname));
        html.replace("%STASSID%", String(config.sta_ssid));
        html.replace("%STAPW%", String(config.sta_password));
        html.replace("%APSSID%", String(config.ap_ssid));
        html.replace("%APPW%", String(config.ap_password));
        
        request->send(200, "text/html", html); });

    // 2. ROUTE: Speichern der Daten (/save)
    server->on("/save", HTTP_POST, [this](AsyncWebServerRequest *request)
               {
        if (request->hasParam("latitude", true) && request->hasParam("longitude", true) && request->hasParam("timezone", true)) {
            config.latitude = request->getParam("latitude", true)->value().toFloat();
            config.longitude = request->getParam("longitude", true)->value().toFloat();
            config.timeZone = request->getParam("timezone", true)->value().toInt();
            config.sta_hostname = request->getParam("sta_hostname", true)->value();
            config.sta_ssid = request->getParam("sta_ssid", true)->value();
            config.sta_password = request->getParam("sta_password", true)->value();
            config.ap_ssid = request->getParam("ap_ssid", true)->value();
            config.ap_password = request->getParam("ap_password", true)->value();
            
            saveConfig(); 
            request->send(200, "text/plain", "Konfiguration gespeichert. Starte neu...");
            delay(1000);
            ESP.restart();
        } else {
            request->send(400, "text/plain", "Fehler: Parameter fehlen.");
        } });

    // 3. STATISCHE ROUTEN: Liefert alle anderen Dateien (style.css etc.) aus LittleFS aus
    server->serveStatic("/", LittleFS, "/");

    // 4. 404-Handler
    server->onNotFound([](AsyncWebServerRequest *request)
                       { request->send(404, "text/html", "<h1>Datei nicht gefunden. Versuche salah.local</h1>"); });

    server->begin();
    Serial.println(String("Webserver gestartet. Gehe zu http://") + config.sta_hostname + ".local");
}