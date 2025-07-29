#include "gfx_setup.h"
#include "../config/config.h"

/** Sets up Arduino GFX and returns gfx class */
Arduino_GFX *setup_gfx()
{
  /* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
  Arduino_DataBus *bus = new Arduino_ESP32SPI(
      PIN_NUM_LCD_DC /* DC */, PIN_NUM_LCD_CS /* CS */,
      PIN_NUM_LCD_SCLK /* SCK */, PIN_NUM_LCD_MOSI /* MOSI */,
      PIN_NUM_LCD_MISO /* MISO */);

  /* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
  Arduino_GFX *gfx = new Arduino_ST7789(
      bus, PIN_NUM_LCD_RST /* RST */, LCD_ROTATION /* rotation */,
      true /* IPS */, LCD_H_RES /* width */, LCD_V_RES /* height */);

  if (!gfx->begin())
  {
    USBSerial.println("gfx->begin() failed!");
  }

  gfx->fillScreen(BLACK);

#ifdef PIN_NUM_LCD_BL
  pinMode(PIN_NUM_LCD_BL, OUTPUT);
  digitalWrite(PIN_NUM_LCD_BL, HIGH);
#endif

  return gfx;
}