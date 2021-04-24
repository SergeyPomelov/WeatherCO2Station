#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(DISPLAY_ADDR, 20U, 4U);

void lcdInit()
{
  lcd.init(SDA, SCL);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0U, 0U);
  analogWrite(BACKLIGHT_PIN, 255U);
}

void lcdSetCursor(const uint8_t col, const uint8_t row)
{
  lcd.setCursor(col, row);
}

void lcdPrint(const String &s)
{
  lcd.print(s);
}

void lcdClear()
{
  lcd.clear();
}

String addSpaces(String input, const uint8_t amount)
{
  String output = String(input.c_str());

  for (int i = 0; i < amount; i++)
  {
    output.concat(' ');
  }
  return output;
}

void print(const uint8_t row, const String str)
{
  uint8_t cols = 20U;
  String out = String(str.c_str());

  if (cols > str.length())
  {
    out = addSpaces(str, cols - str.length());
    //Serial.println("out1 " + out);
  }

  if (cols < str.length())
  {
    out = str.substring(0, cols);
    //Serial.println("out2 " + out);
  }

  lcd.setCursor(0U, row);
  //Serial.println("in " + str);
  
  lcd.print(out);
}

void lcdUpdateData()
{
  backlit = (uint32_t)lux30.getAverage(30) * 8;
  backlit = (backlit < 1023U) ? (int)backlit : 1023U;
  backlit = (backlit < 250U) ? 0U : (int)backlit;

  print(0U, String(insideTemp, 2U) + "C " + String(outsideTemp, 1U) + "C " + (int)lux.getAverage() + "Lux");
  print(1U, "RH " + String(insideHum, 0U) + "% " + String(insidePres) + "hPa");
  print(2U, "CO2 " + String(co2Value) + "ppm");
  print(3U, String(timeStr) + " " + "TVOC " + String((uint16_t)tvocValue.getMedian()));

  analogWrite(BACKLIGHT_PIN, backlit);
}
