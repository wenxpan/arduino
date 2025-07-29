#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "pepper.c"
#include "gfx_setup.h"

Arduino_GFX *gfx = nullptr;

void setup(void)
{
  // Serial.begin(115200);
  // Serial.println("Arduino_GFX Hello World example");
  USBSerial.begin(115200);
  USBSerial.println("USB Arduino_GFX Hello World example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  gfx = setup_gfx();
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef EXAMPLE_PIN_NUM_LCD_BL
  pinMode(EXAMPLE_PIN_NUM_LCD_BL, OUTPUT);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_BL, HIGH);
#endif

  // gfx->setCursor(10, 10);
  // gfx->setTextColor(RED);
  // gfx->println("Hello World!");
  gfx->draw16bitRGBBitmap(0, 0, (const uint16_t *)gimp_image.pixel_data, gimp_image.width, gimp_image.height);

  // delay(5000); // 5 seconds
}

void loop()
{
  // gfx->setCursor(random(gfx->width()), random(gfx->height()));
  // gfx->setTextColor(random(0xffff), random(0xffff));
  // gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
  // gfx->println("Hello World!");

  delay(1000); // 1 second
  Serial.println("Arduino_GFX Hello World example");
}
