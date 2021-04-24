#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <WiFi.h>
#include <Co2.h>
#include <LCD.h>
#include <BME280.h>
#include <TVOC.h>
#include <PM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Scanner.h>
#include <Ntp.h>
#include <Wire.h>
#include <Ticker.h>

boolean status = true;

void update()
{
  const int luxReading = analogRead(LUX_PIN);
  //const int TVOCReading = TVOCread();
  const int TVOCReading = 0;
  lux.add((float)luxReading * 0.64453125);
  lux30.add((float)luxReading * 0.64453125);
  BME280read();
  // Serial.println(String("Co2"));
  Co2read();
  // Serial.println(String("lcdUpdate"));
  lcdUpdateData();

  Serial.print(timeStr + String(" Temp ") + insideTemp + String(" Out Temp ") + outsideTemp  + " Hum " + insideHum + " Press " + insidePres);
  Serial.print(String(" CO2 ") + co2Value + " TVOC " + TVOCReading);
  Serial.println(String(" Backlit ") + backlit + " Lux " + luxReading);
  ESP.wdtFeed();
}

Ticker scannerTicker(scanner, INTERVAL_UPDATE_MS, 0, MILLIS);
Ticker updateTicker(update, INTERVAL_UPDATE_MS, 0, MILLIS);
Ticker ntpTiker(updateTime, INTERVAL_UPDATE_MS, 0, MILLIS);
Ticker sendDataTicker(sendDataDomoticz, INTERVAL_DATA_SEND_MS, 0, MILLIS);
Ticker updateTempTicker(updateTemp, INTERVAL_DATA_GET_MS, 0, MILLIS);
Ticker PMTicker(PMread, INTERVAL_PM_READ_MS, 0, MILLIS);

void initDevice(const String deviceName, const uint8_t displayLine, boolean (*init)())
{
  lcdSetCursor(0U, displayLine);
  lcdPrint(deviceName + "... ");
  Serial.print(deviceName + "... ");
  if (init())
  {
    lcdPrint(F("OK"));
    Serial.println(F("OK"));
  }
  else
  {
    lcdPrint(F("ERR"));
    Serial.println(F("ERR"));
    status = false;
  }
}

void setup()
{

  Serial.begin(76800U);
  Serial.setTimeout(2000U);
  Serial.setDebugOutput(true);
  digitalWrite(LED_BUILTIN, HIGH);
  while (!Serial)
    ;

  pinMode(LUX_PIN, INPUT);
  pinMode(BACKLIGHT_PIN, OUTPUT);

  Serial.print("Startup reason: ");
  Serial.println(ESP.getResetReason());
  
  scannerTicker.start();
  updateTicker.start();
  ntpTiker.start();
  sendDataTicker.start();
  PMTicker.start();
  updateTempTicker.start();

  lcdInit();
  initDevice(String("S8"), 0U, Co2init);
  initDevice(String("BME280"), 1U, BME280init);
  //initDevice(String("CCS811"), 2U, TVOCinit);
  // initDevice(String("S8 OK, PM"), 0U, PMinit);
  initDevice(String("WiFi"), 2U, WiFiconnect);
  initDevice(String("NTP"), 3U, ntpInit);

  if (status)
  {
    lcdClear();
  }
  else
  {
    Serial.println(F("Self-check failed"));
    while (true)
      delay(100);
  }
  
  ESP.wdtDisable();
  ESP.wdtEnable(60 * WDTO_1S);

  tvocValue.add(0.0F);
  updateTemp();

  lcdClear();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  // scannerTicker.update();
  updateTicker.update();
  sendDataTicker.update();
  updateTempTicker.update();
  ntpTiker.update();
  // PMTicker.update();
}
