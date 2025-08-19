#include <JPEGDEC.h>

#include "draw_utils.h"
#include "../pepper.c"
#include "JpegFunc.h"
#include "./GifClass.h"

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

void drawGifFromSD(Arduino_GFX *gfx, GifClass gifClass, const char *fileName)
{
  if (!SD.exists(fileName))
  {
    USBSerial.println("Gif file not found");
    return;
  }

  File gifFile = SD.open(fileName, "r");
  if (!gifFile || gifFile.isDirectory())
  {
    USBSerial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      USBSerial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      uint8_t *buf = (uint8_t *)malloc(gif->width * gif->height);
      if (!buf)
      {
        USBSerial.println(F("buf malloc failed!"));
      }
      else
      {
        int16_t x = (gfx->width() - gif->width) / 2;
        int16_t y = (gfx->height() - gif->height) / 2;

        USBSerial.println(F("GIF video start"));
        int32_t start_ms = millis(), t_delay = 0, delay_until;
        int32_t res = 1;
        int32_t duration = 0, remain = 0;
        while (res > 0)
        {
          t_delay = gif->gce.delay * 10;
          res = gifClass.gd_get_frame(gif, buf);
          if (res < 0)
          {
            USBSerial.println(F("ERROR: gd_get_frame() failed!"));
            break;
          }
          else if (res > 0)
          {
            gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);

            duration += t_delay;
            delay_until = start_ms + duration;
            while (millis() < delay_until)
            {
              delay(1);
              remain++;
            }
          }
        }
        USBSerial.println(F("GIF video end"));
        USBSerial.print(F("Actual duration: "));
        USBSerial.print(millis() - start_ms);
        USBSerial.print(F(", expected duration: "));
        USBSerial.print(duration);
        USBSerial.print(F(", remain: "));
        USBSerial.print(remain);
        USBSerial.print(F(" ("));
        USBSerial.print(100.0 * remain / duration);
        USBSerial.println(F("%)"));

        gifClass.gd_close_gif(gif);
        free(buf);
      }
    }
  }
}