#include "display_manager.h"

DisplayManager::DisplayManager(Arduino_GFX *display)
    : _display(display) {}

bool DisplayManager::begin()
{
  if (!_display)
  {
    USBSerial.println("Display pointer is null");
    return false;
  }

  if (!_display->begin())
  {
    USBSerial.println("Display initialization failed");
    return false;
  }

  _display->fillScreen(BLACK);
  _display->setRotation(LCD_ROTATION);

#ifdef PIN_NUM_LCD_BL
  pinMode(PIN_NUM_LCD_BL, OUTPUT);
  digitalWrite(PIN_NUM_LCD_BL, HIGH);
#endif

  _initialized = true;
  return true;
}

void DisplayManager::clearScreen(uint16_t color)
{
  if (_initialized)
  {
    _display->fillScreen(color);
  }
}

void DisplayManager::drawImage(const uint16_t *bitmap,
                               int16_t x, int16_t y,
                               uint16_t width, uint16_t height)
{
  if (!_initialized || !bitmap)
    return;

  _display->draw16bitRGBBitmap(x, y, bitmap, width, height);
}

void DisplayManager::drawImageCentered(const uint16_t *bitmap,
                                       uint16_t width, uint16_t height)
{
  if (!_initialized)
    return;

  int16_t x = (getWidth() - width) / 2;
  int16_t y = (getHeight() - height) / 2;
  drawImage(bitmap, x, y, width, height);
}

uint16_t DisplayManager::getWidth() const
{
  return _initialized ? _display->width() : 0;
}

uint16_t DisplayManager::getHeight() const
{
  return _initialized ? _display->height() : 0;
}