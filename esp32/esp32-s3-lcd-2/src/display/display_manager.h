#pragma once
#include <Arduino_GFX_Library.h>
#include "../config/config.h"

class DisplayManager
{
public:
  // Constructor with dependency injection
  DisplayManager(Arduino_GFX *display = nullptr);

  // Initialize display hardware
  bool begin();

  // Drawing methods
  void clearScreen(uint16_t color = BLACK);
  void drawImage(const uint16_t *bitmap, int16_t x, int16_t y,
                 uint16_t width, uint16_t height);
  void drawImageCentered(const uint16_t *bitmap,
                         uint16_t width, uint16_t height);

  // Utility methods
  void setBrightness(uint8_t level);
  uint16_t getWidth() const;
  uint16_t getHeight() const;

private:
  Arduino_GFX *_display;
  bool _initialized = false;

  void initBacklight();
};