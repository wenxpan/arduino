#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "pepper.c"
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./display/display_manager.h"

DisplayManager display(setup_gfx());

void setup(void)
{
  USBSerial.begin(115200);
  USBSerial.println("Starting");

  if (!display.begin())
  {
    USBSerial.println("Failed to initialize display!");
    while (1)
      ; // Halt on failure
  }

  display.drawImageCentered(
      (const uint16_t *)gimp_image.pixel_data,
      gimp_image.width,
      gimp_image.height);
  // // Init Display
  // gfx = setup_gfx();
  // if (!gfx->begin())
  // {
  //   USBSerial.println("gfx->begin() failed!");
  // }

  // draw_map(gfx, (const uint16_t *)gimp_image.pixel_data,
  //          gimp_image.width, gimp_image.height);
}

void loop()
{
  delay(10000); // 1 second
}
