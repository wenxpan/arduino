#include "draw_utils.h"

void draw_hello_world(Arduino_GFX *gfx, boolean clearScreen)
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

void draw_map(Arduino_GFX *gfx, const uint16_t *bitmap, int width, int height)
{
  if (!gfx)
    return;

  gfx->fillScreen(BLACK); // Clear screen first
  gfx->draw16bitRGBBitmap(0, 0, bitmap, width, height);
}