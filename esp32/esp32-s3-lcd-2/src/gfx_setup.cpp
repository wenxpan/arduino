#include "gfx_setup.h"

Arduino_GFX *setup_gfx()
{
  /* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
  Arduino_DataBus *bus = new Arduino_ESP32SPI(
      EXAMPLE_PIN_NUM_LCD_DC /* DC */, EXAMPLE_PIN_NUM_LCD_CS /* CS */,
      EXAMPLE_PIN_NUM_LCD_SCLK /* SCK */, EXAMPLE_PIN_NUM_LCD_MOSI /* MOSI */,
      EXAMPLE_PIN_NUM_LCD_MISO /* MISO */);

  /* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
  Arduino_GFX *gfx = new Arduino_ST7789(
      bus, EXAMPLE_PIN_NUM_LCD_RST /* RST */, EXAMPLE_LCD_ROTATION /* rotation */,
      true /* IPS */, EXAMPLE_LCD_H_RES /* width */, EXAMPLE_LCD_V_RES /* height */);

  return gfx;
}