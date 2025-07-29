#ifndef GFX_SETUP_H
#define GFX_SETUP_H

#include <Arduino_GFX_Library.h>

Arduino_GFX *setup_gfx();

// Configuration defines
#define EXAMPLE_PIN_NUM_LCD_SCLK 39
#define EXAMPLE_PIN_NUM_LCD_MOSI 38
#define EXAMPLE_PIN_NUM_LCD_MISO 40
#define EXAMPLE_PIN_NUM_LCD_DC 42
#define EXAMPLE_PIN_NUM_LCD_RST -1
#define EXAMPLE_PIN_NUM_LCD_CS 45
#define EXAMPLE_PIN_NUM_LCD_BL 1

#define EXAMPLE_LCD_ROTATION 1
#define EXAMPLE_LCD_H_RES 240
#define EXAMPLE_LCD_V_RES 320

#endif