#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"

#include <JPEGDEC.h>
#include <SD.h>
#include <SD_MMC.h>
#include "./display/JpegFunc.h"

#define JPEG_FILENAME "/tvsignal.jpg"

Arduino_GFX *gfx = nullptr;

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // USBSerial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup(void)
{
  USBSerial.begin(115200);
  USBSerial.println("Starting");

  // Init Display
  gfx = setupGfx();

  // drawExamplePepper(gfx);

  setupSD();

  logSDInfo();
  drawJpegFromSD(gfx, JPEG_FILENAME, jpegDrawCallback);

  delay(5 * 1000);
}

void loop()
{
  delay(10 * 1000);
}
