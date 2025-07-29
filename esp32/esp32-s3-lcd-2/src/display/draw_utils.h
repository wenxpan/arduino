#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include <Arduino_GFX_Library.h>

void draw_hello_world(Arduino_GFX *gfx, boolean clearScreen = false);

/**
 * @brief draw an example PROGMEM pepper image
 */
void draw_example_pepper(Arduino_GFX *gfx);

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
 * `draw_map(gfx, (const uint16_t *)gimp_image.pixel_data,
 * imp_image.width, gimp_image.height);`
 ******************************************************************************/
void draw_map(Arduino_GFX *gfx, const uint16_t *bitmap, int width, int height);

#endif