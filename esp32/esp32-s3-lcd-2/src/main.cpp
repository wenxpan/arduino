#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>
#include "JpegFunc.h"

#define JPEG_FILENAME "/tvsignal.jpg"

Arduino_GFX *gfx = nullptr;

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup_jpg()
{
/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL))
#elif defined(TARGET_RP2040) || defined(PICO_RP2350)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#elif defined(ESP32)
  // if (!FFat.begin())
  if (!LittleFS.begin())
  // if (!SPIFFS.begin())
  // SPI.begin(12 /* CLK */, 13 /* D0/MISO */, 11 /* CMD/MOSI */);
  // if (!SD.begin(10 /* CS */, SPI))
  // pinMode(10 /* CS */, OUTPUT);
  // digitalWrite(10 /* CS */, HIGH);
  // SD_MMC.setPins(12 /* CLK */, 11 /* CMD/MOSI */, 13 /* D0/MISO */);
  // if (!SD_MMC.begin("/root", true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
  // SD_MMC.setPins(12 /* CLK */, 11 /* CMD/MOSI */, 13 /* D0/MISO */, 14 /* D1 */, 15 /* D2 */, 10 /* D3/CS */);
  // if (!SD_MMC.begin("/root", false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(ESP8266)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#else
  if (!SD.begin())
#endif
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
  else
  {
    unsigned long start = millis();
    jpegDraw(JPEG_FILENAME, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
    Serial.printf("Time used: %lu\n", millis() - start);
  }
}

void setup(void)
{
  USBSerial.begin(115200);
  USBSerial.println("Starting");

  // Init Display
  gfx = setup_gfx();

  draw_example_pepper(gfx);

  // sd_setup();

  // unsigned long start = millis();
  // jpegDraw(JPEG_FILENAME, jpegDrawCallback, true /* useBigEndian */,
  //          0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  // USBSerial.printf("Time used: %lu\n", millis() - start);

  delay(5000);
}

void loop()
{
  //  int w = gfx->width();
  // int h = gfx->height();

  // unsigned long start = millis();

  // jpegDraw(JPEG_FILENAME, jpegDrawCallback, true /* useBigEndian */,
  //          random(w * 2) - w /* x */,
  //          random(h * 2) - h /* y */,
  //          w /* widthLimit */, h /* heightLimit */);

  // Serial.printf("Time used: %lu\n", millis() - start);

  delay(10000); // 1 second
}
