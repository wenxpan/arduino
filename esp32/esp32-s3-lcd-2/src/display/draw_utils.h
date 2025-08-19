#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include <Arduino_GFX_Library.h>
#include <JPEGDEC.h>
#include "./GifClass.h"

void drawHelloWorld(Arduino_GFX *gfx, boolean clearScreen = false);

/**
 * @brief draw an example PROGMEM pepper image
 */
void drawExamplePepper(Arduino_GFX *gfx);

/*******************************************************************************
 * PROGMEM Image Viewer
 *
 * To create custom image:
 * 1. Open image in the GIMP
 * 2. Resize image to fit for the display and MCU memory
 * 3. Export Image as C-Source
 * 4. Uncheck all option and check "Save as RGB565 (16-bit)"
 * 5. Revise exported file just like "xxx.c"
 *
 * Example use:
 * `drawBitmap(gfx, (const uint16_t *)gimp_image.pixel_data,
 * imp_image.width, gimp_image.height);`
 ******************************************************************************/
void drawBitmap(Arduino_GFX *gfx, const uint16_t *bitmap, int width, int height);

void drawJpegFromSD(Arduino_GFX *gfx, const char *fileName, JPEG_DRAW_CALLBACK *jpegDrawCallback);

void drawGifFromSD(Arduino_GFX *gfx, GifClass gifClass, const char *fileName);

#endif