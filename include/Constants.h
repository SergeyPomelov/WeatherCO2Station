#include <Secrets.h>

#define DEBUG           true
#define DEBUG_NTPClient true
#define DISPLAY_ADDR    0x27
#define ADS1115_ADDR    0x48
#define CCS811_ADDR     0x5A
#define LUX_PIN         A0
#define S8_TX           D3
#define S8_RX           D4
#define BACKLIGHT_PIN   D5
#define PM10_PIN        D7
#define PM25_PIN        D8

#define UTC_OFFSET_SEC  60U * 60U * 7U

#define INTERVAL_UPDATE_MS      1U * 1000U
#define INTERVAL_DATA_SEND_MS   60U * 1000U
#define INTERVAL_DATA_GET_MS    300U * 1000U
#define INTERVAL_PM_READ_MS     30U * 1000U