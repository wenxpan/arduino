#include <JPEGDEC.h>

#include "draw_utils.h"
#include "../pepper.c"
#include "JpegFunc.h"
#include "./GifClass.h"

// Time display variables
unsigned long lastTimeUpdate = 0;
const unsigned long timeUpdateInterval = 30000; // Update time every second
char timeStr[20];

uint16_t *timeOverlayBuffer = nullptr;
int overlayWidth = 0, overlayHeight = 0;
int overlayX = 0, overlayY = 0;

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

void drawTimeOverlay(Arduino_GFX *gfx)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    USBSerial.println("Failed to obtain time");
    return;
  }

  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  // strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  // Set text properties
  gfx->setTextSize(6);
  gfx->setTextColor(WHITE);

  // Calculate text dimensions
  int16_t x1, y1;
  uint16_t w, h;
  gfx->getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);

  // Position at bottom left corner with some padding
  int x = 30;                     // 10px from left
  int y = gfx->height() - h - 60; // 10px from bottom

  // Draw semi-transparent background (50% opacity)
  // uint16_t bgColor = 0x1082; // Dark gray in RGB565

  // gfx->fillRect(x - 5, y - 2, w + 10, h + 4, bgColor);

  // Draw the time text
  gfx->setCursor(x, y + h - 2); // Adjust for proper vertical alignment
  gfx->print(timeStr);
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
        unsigned long lastFrameTime = millis();

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

            // Draw time overlay on top of the GIF
            drawTimeOverlay(gfx);

            duration += t_delay;
            delay_until = start_ms + duration;

            // Update time display periodically
            if (millis() - lastTimeUpdate >= timeUpdateInterval)
            {
              lastTimeUpdate = millis();
              // Redraw time to keep it updated
              drawTimeOverlay(gfx);
            }

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