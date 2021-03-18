
#include <Arduino.h>
#include <Constants.h>
#include <Data.h>
#include <SoftwareSerial.h>

const byte s8_co2[8] = {0xfe, 0x04, 0x00, 0x03, 0x00, 0x01, 0xd5, 0xc5};
const byte s8_fwver[8] = {0xfe, 0x04, 0x00, 0x1c, 0x00, 0x01, 0xe4, 0x03};
const byte s8_id_hi[8] = {0xfe, 0x04, 0x00, 0x1d, 0x00, 0x01, 0xb5, 0xc3};
const byte s8_id_lo[8] = {0xfe, 0x04, 0x00, 0x1e, 0x00, 0x01, 0x45, 0xc3};
const byte s8_id_lol[8] = {0xfe, 0x04, 0x00, 0x00, 0x00, 0x04, 0xe5, 0xc3};
SoftwareSerial swSer(S8_RX, S8_TX, false); // RX, TX
byte buf[10];

void myread(int n)
{
  int i;
  memset(buf, 0, sizeof(buf));
  for (i = 0; i < n;)
  {
    if (swSer.available() > 0)
    {
      buf[i] = swSer.read();
      i++;
    }
    yield();
    delay(10);
  }
}

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(byte *buf, int len)
{
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--)
    { // Loop over each bit
      if ((crc & 0x0001) != 0)
      {            // If the LSB is set
        crc >>= 1; // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else         // Else LSB is not set
        crc >>= 1; // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

boolean Co2init()
{
  swSer.begin(9600);
  delay(10);
  Serial.print("S8 Sensor ID: ");
  swSer.write(s8_id_hi, 8);
  myread(7);
  Serial.printf("%02x%02x", buf[3], buf[4]);
  swSer.write(s8_id_lo, 8);
  myread(7);
  Serial.printf("%02x%02x", buf[3], buf[4]);
  Serial.println("");

  swSer.write(s8_fwver, 8);
  myread(7);
  Serial.printf("S8 Firmware: %d.%d", buf[3], buf[4]);
  return buf[3] == 1U;
}

uint16_t Co2read()
{
  uint16_t crc, got, co2;

  swSer.write(s8_co2, 8);
  myread(7);
  co2 = (uint16_t)buf[3] * 256 + (uint16_t)buf[4];
  crc = ModRTU_CRC(buf, 5);
  got = (uint16_t)buf[5] + (uint16_t)buf[6] * 256;
  if (crc != got)
  {
    Serial.print("Invalid checksum.  Expect: ");
    Serial.print(crc, HEX);
    Serial.print("  Got: ");
    Serial.println(got, HEX);
  }
  else
  {
    // Serial.print("CO2: ");
    // Serial.println(co2);
    co2Value = co2;
    // Serial.printf("%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
  }
  return co2;
}