#include "Seeed_BME280.h"
#include <ss_oled.h>

char tmp[256];

BME280 bme280;
float MSL = 102360;
uint32_t t0;
#define DelaySomeMore 30000

static uint8_t ucBackBuffer[1024];
#define SDA_PIN PIN_WIRE_SDA
#define SCL_PIN PIN_WIRE_SCL
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 1
#define INVERT 0
#define USE_HW_I2C 1
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
SSOLED ssoled;

void setup() {
  Serial.begin(230400);
  int rc;
  rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L); // use standard I2C bus at 400Khz
  if (rc != OLED_NOT_FOUND) {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0, 0, 0, msgs[rc], FONT_NORMAL, 0, 1);
    oledSetBackBuffer(&ssoled, ucBackBuffer);
    delay(2000);
  }
  sprintf(tmp, "MSL: %.2f HPa", MSL / 100.0);
  oledFill(&ssoled, 0x0, 1);
  oledWriteString(&ssoled, 0, 0, 0, (char *)"BME280 H750 test", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0, 0, 1, (char *)" Written by Kongduino", FONT_SMALL, 1, 1);
  oledWriteString(&ssoled, 0, 0, 3, (char *)"**Demo**", FONT_LARGE, 0, 1);
  oledWriteString(&ssoled, 0, 0, 7, tmp, FONT_NORMAL, 0, 1);
  oledSetBackBuffer(&ssoled, ucBackBuffer);
  Serial.println("BME280 H750 test");
  Serial.println(tmp);
  if (!bme280.init()) {
    Serial.println("Device error!");
    oledFill(&ssoled, 0x0, 1);
    oledWriteString(&ssoled, 0, 0, 3, (char *)"*Error*", FONT_LARGE, 0, 1);
    oledSetBackBuffer(&ssoled, ucBackBuffer);
    while (1) ;
  }
  delay(2000);
  showData();
}

void loop() {
  if (millis() - t0 > DelaySomeMore) {
    showData();
  }
  if (Serial.available()) {
    uint8_t ix = 0;
    while (Serial.available()) tmp[ix++] = Serial.read();
    tmp[ix] = 0;
    float mymsl = atof(tmp);
    if (mymsl > 87000.0 && mymsl < 108480.0) {
      MSL = mymsl;
      Serial.printf("MSL set to %.2f\n", MSL);
      showData();
    }
  }
}

void showData() {
  float temperature, pressure, altitude, humidity;
  // get and print temperatures
  temperature = bme280.getTemperature();
  oledFill(&ssoled, 0x0, 1);
  sprintf(tmp, "Temp: %.2f C", temperature);
  oledWriteString(&ssoled, 0, 0, 0, tmp, FONT_NORMAL, 0, 1);
  Serial.println(tmp);

  // get and print humidity data
  humidity = bme280.getHumidity();
  sprintf(tmp, "RH:   %.2f%%", humidity);
  oledWriteString(&ssoled, 0, 0, 1, tmp, FONT_NORMAL, 0, 1);
  Serial.println(tmp);

  // get and print atmospheric pressure data
  pressure = bme280.getPressure();
  sprintf(tmp, "HPa:  %.2f", pressure / 100.0);
  oledWriteString(&ssoled, 0, 0, 2, tmp, FONT_NORMAL, 0, 1);
  Serial.println(tmp);

  // get and print altitude data
  altitude = bme280.calcAltitude(pressure, MSL);
  sprintf(tmp, "Alt:  %.2f m", altitude);
  oledWriteString(&ssoled, 0, 0, 3, tmp, FONT_NORMAL, 0, 1);
  Serial.println(tmp);

  sprintf(tmp, "MSL: %.2f HPa", MSL / 100.0);
  oledWriteString(&ssoled, 0, 0, 7, tmp, FONT_NORMAL, 0, 1);
  oledSetBackBuffer(&ssoled, ucBackBuffer);
  t0 = millis();
}
