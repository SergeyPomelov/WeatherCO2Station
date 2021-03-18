#include <Arduino.h>
#include <Constants.h>
#include <Data.h>

int val = 0;
float valDUSTPM25 = 0.0;
float valDUSTPM10 = 0.0;
uint32_t duration;
uint32_t starttime;
uint32_t endtime;
uint32_t sampletime_ms = 3000;
uint32_t lowpulseoccupancy = 0;
float ratio = 0;
long concentrationPM25 = 0;
long concentrationPM10 = 0;

boolean PMinit()
{
  pinMode(PM10_PIN, INPUT);
  pinMode(PM25_PIN, INPUT);
  return true;
}

long getPM(int DUST_SENSOR_DIGITAL_PIN)
{
  starttime = millis();
  while (1)
  {
    duration = pulseIn(DUST_SENSOR_DIGITAL_PIN, LOW);
    lowpulseoccupancy += duration;
    endtime = millis();

    if ((endtime - starttime) >= sampletime_ms)
    {
      ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
      long concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
      lowpulseoccupancy = 0;
      return (concentration);
    }
  }
}

void PMread()
{
  //get PM 1.0 - density of particles over 1 µm.
  concentrationPM10 = getPM(PM10_PIN);

  //ppmv=mg/m3 * (0.08205*Tmp)/Molecular_mass
  //0.08205   = Universal gas constant in atm·m3/(kmol·K)
  //external temperature, if you can replace this with a DHT11 or better
  // long ppmv = (concentrationPM10 * 0.0283168 / 100 / 1000) * (0.08205 * insideTemp) / 0.01;
  if ((ceil(concentrationPM10) != PM10Reading) && ((long)concentrationPM10 > 0))
  {
    PM10Reading = ceil(concentrationPM10 / 100.0);
  }
}
