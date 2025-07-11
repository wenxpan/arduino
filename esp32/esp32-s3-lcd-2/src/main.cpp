#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <secrets.h>
#include <WiFi.h>
#include <JPEGDEC.h>

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

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = new Arduino_ESP32SPI(
  EXAMPLE_PIN_NUM_LCD_DC /* DC */, EXAMPLE_PIN_NUM_LCD_CS /* CS */,
  EXAMPLE_PIN_NUM_LCD_SCLK /* SCK */, EXAMPLE_PIN_NUM_LCD_MOSI /* MOSI */, EXAMPLE_PIN_NUM_LCD_MISO /* MISO */);

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ST7789(
  bus, EXAMPLE_PIN_NUM_LCD_RST /* RST */, EXAMPLE_LCD_ROTATION /* rotation */, true /* IPS */,
  EXAMPLE_LCD_H_RES /* width */, EXAMPLE_LCD_V_RES /* height */);

/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// Image URL to download
const char* imageUrl = "https://picsum.photos/id/237/200/300.jpg";

// JPEG decoder instance
JPEGDEC jpeg;

// Buffer for downloaded image
uint8_t* jpegBuffer = nullptr;
size_t jpegBufferSize = 0;

// Function to draw the JPEG to the display
int JPEGDraw(JPEGDRAW *pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup(void)
{
  Serial.begin(115200);
  USBSerial.begin(115200);

  while(!Serial && !USBSerial);

  USBSerial.println("This is USBSerial (CDC) output");

  // Init Display
  if (!gfx->begin())
  {
    USBSerial.println("gfx->begin() failed!");
  }

#ifdef EXAMPLE_PIN_NUM_LCD_BL
  pinMode(EXAMPLE_PIN_NUM_LCD_BL, OUTPUT);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_BL, HIGH);
#endif

  gfx->fillScreen(YELLOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  USBSerial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USBSerial.print(".");
  }
  USBSerial.println();
  USBSerial.print("Connected with IP: ");
  USBSerial.println(WiFi.localIP());
}

void loop()
{
  USBSerial.println(WiFi.localIP());
  delay(1000); // 1 second
}
