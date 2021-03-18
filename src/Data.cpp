#include <Arduino.h>
#include <RunningMedian.h>

float outsideTemp;
float insideTemp;
String timeStr;
float insideHum;
uint16_t insidePres;
RunningMedian lux30(30U);
RunningMedian lux(5U);
uint16_t co2Value;
RunningMedian tvocValue(10U);
uint16_t PM10Reading;
uint16_t PM25Reading;
uint32_t backlit;