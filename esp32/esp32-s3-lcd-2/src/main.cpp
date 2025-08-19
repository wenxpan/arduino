#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"

#include <JPEGDEC.h>
#include <SD.h>
#include <SD_MMC.h>
#include "./display/JpegFunc.h"
#include "./display/GifClass.h"

static GifClass gifClass;
Arduino_GFX *gfx = nullptr;
File root;
String imageFiles[50]; // Array to hold image filenames
int imageCount = 0;
int currentImage = 0;
unsigned long lastChangeTime = 0;
const unsigned long rotationInterval = 5000; // 5 seconds

#define GIF_FILENAME "/rotating-earth.gif"

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // USBSerial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}
void loadImageList()
{
  root = SD.open("/");
  File file = root.openNextFile();
  imageCount = 0;

  while (file && imageCount < 50)
  {
    if (!file.isDirectory())
    {
      String filename = file.name();
      // Filter hidden files
      if ((filename.endsWith(".jpg") || filename.endsWith(".jpeg")) &&
          !filename.startsWith("._") &&
          !filename.startsWith("."))
      {
        // Store with full path
        imageFiles[imageCount] = "/" + filename;
        USBSerial.println("Found valid image: " + imageFiles[imageCount]);
        imageCount++;
      }
      else
      {
        USBSerial.println("Skipping: " + filename);
      }
    }
    file = root.openNextFile();
  }

  USBSerial.printf("Found %d valid images\n", imageCount);
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

  // // Load list of JPEG images
  // loadImageList();

  // // Display first image if available
  // if (imageCount > 0)
  // {
  //   drawJpegFromSD(gfx, imageFiles[currentImage].c_str(), jpegDrawCallback);
  //   lastChangeTime = millis();
  // }
  // else
  // {
  //   USBSerial.println("No JPEG images found!");
  // }
}

void loop()
{
  // if (imageCount > 0)
  // {
  //   unsigned long currentTime = millis();

  //   // Check if it's time to change image
  //   if (currentTime - lastChangeTime >= rotationInterval)
  //   {
  //     lastChangeTime = currentTime;

  //     // Move to next image
  //     currentImage = (currentImage + 1) % imageCount;

  //     // Display the image
  //     USBSerial.println("Displaying: " + imageFiles[currentImage]);
  //     drawJpegFromSD(gfx, imageFiles[currentImage].c_str(), jpegDrawCallback);
  //   }
  // }

  // delay(1 * 1000);
  File gifFile = SD.open(GIF_FILENAME, "r");
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
