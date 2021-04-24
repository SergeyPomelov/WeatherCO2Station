#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <SparkFunCCS811.h>

CCS811 ccs(CCS811_ADDR);

void printSensorError()
{
  uint8_t error = ccs.getErrorRegister();

  if (error == 0xFF) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
  }
  else
  {
    Serial.print("Error: ");
    if (error & 1 << 5)
      Serial.print("HeaterSupply");
    if (error & 1 << 4)
      Serial.print("HeaterFault");
    if (error & 1 << 3)
      Serial.print("MaxResistance");
    if (error & 1 << 2)
      Serial.print("MeasModeInvalid");
    if (error & 1 << 1)
      Serial.print("ReadRegInvalid");
    if (error & 1 << 0)
      Serial.print("MsgInvalid");
    Serial.println();
  }
}

boolean TVOCinit()
{
  boolean result = ccs.begin();
  if (ccs.checkForStatusError())
  {
    printSensorError();
  }
  return result;
}

int TVOCread()
{
  int TVOCReading = 0;
  if (ccs.dataAvailable())
  {
    ccs.setEnvironmentalData(insideHum, insideTemp);
    ccs.readAlgorithmResults();
    TVOCReading = ccs.getTVOC();
    tvocValue.add((float)TVOCReading);
    // Serial.print(" TVOC read " + String(tvocValue) + String("temp ") + ccs.getTemperature() + String("co2 ") + ccs.s);
  }
  else if (ccs.checkForStatusError())
  {
    printSensorError();
  }
  return TVOCReading;
}
