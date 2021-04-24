#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

HTTPClient http;
WiFiClient wifi;

void updateTemp()
{
  String url = "http://" + String(DOMOTICZ_IP) + ":" + DOMOTICZ_PORT + "/json.htm?type=devices&rid=194";
  if (http.begin(wifi, url))
  {
    http.addHeader("Accept", "*/*");
    http.addHeader("Host", DOMOTICZ_IP);
    http.addHeader("Upgrade-Insecure-Requests", "1");
    http.addHeader("User-Agent", "ESP8266 Node Mcu 2 Weather Station");
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = http.getString();
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);
        outsideTemp = doc["result"][0]["Temp"];
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else
  {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}

boolean WiFiconnect()
{

  if (WiFi.status() != WL_CONNECTED || WiFi.SSID() != AP_SSID)
  {
    Serial.println(F("Initialising Wifi..."));
    WiFi.mode(WIFI_STA);
    IPAddress ip4 = ipaddr_addr("8.8.8.8"); //dns 1
    IPAddress ip5 = ipaddr_addr("8.8.4.4"); //dns 2
    WiFi.config(IP, GATE, MASK, ip4, ip5);
    WiFi.begin(AP_SSID, AP_PASS);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print('.');
  }
  Serial.print(" Connected! IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

int updateDevice(const uint16_t idx, const String value, const boolean terminal)
{

  int httpCode = 0;
  String url = "http://" + String(DOMOTICZ_IP) + ":" + DOMOTICZ_PORT +
               "/json.htm?type=command&param=udevice&idx=" + String(idx) +
               "&nvalue=" + value + "&svalue=" + value;

  http.begin(wifi, url);
  http.addHeader("Accept", "*/*");
  http.addHeader("Host", DOMOTICZ_IP);
  http.addHeader("Upgrade-Insecure-Requests", "1");
  http.addHeader("User-Agent", "ESP8266 Node Mcu 2 Weather Station");

  if (terminal)
  {
    http.addHeader("Connection", "close");
  }

  Serial.println((char *)url.c_str());
  httpCode = http.GET();

  if (httpCode > 0)
  {
    // Serial.printf("Domoticz send code: %d\n", httpCode);
    if (httpCode != HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  else
  {
    Serial.printf("Domoticz send failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return httpCode;
}

void sendDataDomoticz()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Domoticz send begin...\n");
    updateDevice(187U, String(co2Value), false);
    // updateDevice(190U, String(PM10Reading), false);
    updateDevice(191U, String((uint16_t)tvocValue.getMedian()), true);
  }
}
