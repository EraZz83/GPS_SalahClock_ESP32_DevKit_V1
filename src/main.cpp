#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "TM1637Display.h"
#include <time.h>

//Hardware
#define display_1_DIO 13
#define display_1_CLK 12
#define display_2_DIO 14
#define display_2_CLK 27
#define display_3_DIO 26
#define display_3_CLK 25
#define display_4_DIO 33
#define display_4_CLK 32
#define display_5_DIO 35
#define display_5_CLK 34
#define display_6_DIO 23
#define display_6_CLK 22

//Constants
#define distRefreshInterval 500 //ms
#define AutoResetBrightness 5000 //ms

//Wifi Connection
const char* ssid = "TurkishAirlines_";
const char* password = "123Polizei";

//Timeoffset
const int gmtOffset_sec = 3600;
const int daylightOffset_sec = 0;

//Network Time Server
const char* ntpServer = "pool.ntp.org";

int actTime;
int oldTimeMin;
int oldTimeSec;
int brightness = 1;
unsigned long oldMilisRefresh;
unsigned long oldMilisBrightnessChange;
long duration;
struct tm timeinfo;

bool refreshDisplay = true;

TM1637Display display_1(display_1_CLK, display_1_DIO);
TM1637Display display_2(display_2_CLK, display_2_DIO);
TM1637Display display_3(display_3_CLK, display_3_DIO);
TM1637Display display_4(display_4_CLK, display_4_DIO);
TM1637Display display_5(display_5_CLK, display_5_DIO);
TM1637Display display_6(display_6_CLK, display_6_DIO);


void setup(void) {
  Serial.begin(9600);
  display_1.clear();
  display_1.setBrightness(brightness, true);
  display_2.clear();
  display_2.setBrightness(brightness, true);
  display_3.clear();
  display_3.setBrightness(brightness, true);
  display_4.clear();
  display_4.setBrightness(brightness, true);
  display_5.clear();
  display_5.setBrightness(brightness, true);
  display_6.clear();
  display_6.setBrightness(brightness, true);
  
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!MDNS.begin("salah")) // salah.local
  {
    Serial.println("Error setting up mDNS.");
  }
  MDNS.addService("http", "tcp", 80);
}

void printLocalTime()
{
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  actTime = timeinfo.tm_hour * 100;
  actTime += timeinfo.tm_min;

  if ((oldTimeSec != timeinfo.tm_sec))
  {
    oldTimeMin = timeinfo.tm_min;
    oldTimeSec = timeinfo.tm_sec;
    refreshDisplay = true;
    Serial.println("Timechange");
  }
}

void refreshDisplayValueBrightness()
{
  if (refreshDisplay)
  {
    if (brightness > 0)
    {
      display_1.setBrightness(brightness, true);
    }
    else
    {
      display_1.setBrightness(brightness, false);
    }
    display_1.showNumberDecEx(actTime, 0b01000000, true);
    refreshDisplay = false;
  }
}
 
void loop(void) {
  ArduinoOTA.handle();
  printLocalTime();
  refreshDisplayValueBrightness();
  //refreshDisplayValueBrightness(display_1);
  //refreshDisplayValueBrightness(display_2);
  //refreshDisplayValueBrightness(display_3);
  //refreshDisplayValueBrightness(display_4);
  //refreshDisplayValueBrightness(display_5);
  //refreshDisplayValueBrightness(display_6);
}
