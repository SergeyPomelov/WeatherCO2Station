#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", UTC_OFFSET_SEC, 60000);

boolean ntpInit()
{
  timeClient.begin();
  return timeClient.forceUpdate();
}

String getFormattedTime()
{
  unsigned long hours = timeClient.getHours();
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = timeClient.getMinutes();
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  return hoursStr + ":" + minuteStr;
}

String updateTime()
{
  // Serial.println(timeClient.update());
  timeStr = getFormattedTime();
  // Serial.println(getFormattedTime());
  return timeStr;
}