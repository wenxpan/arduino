#include <JPEGDEC.h>

#include "draw_utils.h"
#include "../pepper.c"
#include "JpegFunc.h"

void drawHelloWorld(Arduino_GFX *gfx, boolean clearScreen)
{
  if (!gfx)
    return;

  if (clearScreen)
  {
    gfx->fillScreen(BLACK);
  }

  gfx->setCursor(random(gfx->width()), random(gfx->height()));
  gfx->setTextColor(random(0xffff), random(0xffff));
  gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
  gfx->println("Hello World!");
}

void drawBitmap(Arduino_GFX *gfx, const uint16_t *bitmap, int width, int height)
{
  if (!gfx)
    return;

  gfx->fillScreen(BLACK); // Clear screen first
  gfx->draw16bitRGBBitmap(0, 0, bitmap, width, height);
}

void drawExamplePepper(Arduino_GFX *gfx)
{
  drawBitmap(gfx, (const uint16_t *)gimp_image.pixel_data,
             gimp_image.width, gimp_image.height);
}

void drawJpegFromSD(Arduino_GFX *gfx, const char *fileName, JPEG_DRAW_CALLBACK *jpegDrawCallback)
{
  if (!SD.exists(fileName))
  {
    USBSerial.println("JPEG file not found");
    return;
  }

  unsigned long start = millis();
  jpegDraw(fileName, jpegDrawCallback, true,
           0, 0, gfx->width(), gfx->height());
  USBSerial.printf("Time used: %lu ms\n", millis() - start);
}