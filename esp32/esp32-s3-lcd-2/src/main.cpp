#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "pepper.c"
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"

Arduino_GFX *gfx = nullptr;

void setup(void)
{
  USBSerial.begin(115200);
  USBSerial.println("Starting");

  // Init Display
  gfx = setup_gfx();

  draw_map(gfx, (const uint16_t *)gimp_image.pixel_data,
           gimp_image.width, gimp_image.height);

  sd_setup();
  test_sd();
}

void loop()
{
  delay(10000); // 1 second
}
