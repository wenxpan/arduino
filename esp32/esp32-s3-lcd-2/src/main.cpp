#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <secrets.h>
#include <WiFi.h>
#include <JPEGDEC.h>
#include <HTTPClient.h>

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
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// Image URL to download
const char *imageUrl = "https://picsum.photos/id/237/200/300.jpg";

// JPEG decoder instance
JPEGDEC jpeg;

// Buffer for downloaded image
uint8_t *jpegBuffer = nullptr;
size_t jpegBufferSize = 0;

// Function to draw the JPEG to the display
int JPEGDraw(JPEGDRAW *pDraw)
{
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup(void)
{
  Serial.begin(115200);
  USBSerial.begin(115200);

  while (!Serial && !USBSerial)
    ;

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
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    USBSerial.print(".");
  }
  USBSerial.println();
  USBSerial.print("Connected with IP: ");
  USBSerial.println(WiFi.localIP());

  // Download the image
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(imageUrl);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      // Get the size of the image
      jpegBufferSize = http.getSize();

      // Allocate buffer
      jpegBuffer = (uint8_t *)ps_malloc(jpegBufferSize);
      if (!jpegBuffer)
      {
        USBSerial.println("Failed to allocate memory for JPEG buffer");
        return;
      }

      // Get the image data
      WiFiClient *stream = http.getStreamPtr();
      size_t bytesRead = 0;
      while (http.connected() && (bytesRead < jpegBufferSize))
      {
        size_t bytesAvailable = stream->available();
        if (bytesAvailable)
        {
          bytesRead += stream->readBytes(jpegBuffer + bytesRead, bytesAvailable);
        }
        delay(1);
      }

      USBSerial.printf("Downloaded %d bytes\n", bytesRead);

      // Decode and display the JPEG
      if (jpeg.openRAM(jpegBuffer, jpegBufferSize, JPEGDraw))
      {
        USBSerial.printf("Image dimensions: %d x %d\n", jpeg.getWidth(), jpeg.getHeight());

        // Calculate scaling to fit the display
        int scale = min(
            EXAMPLE_LCD_H_RES / jpeg.getWidth(),
            EXAMPLE_LCD_V_RES / jpeg.getHeight());
        scale = max(1, scale); // Don't scale below 1

        // Center the image
        int x = (EXAMPLE_LCD_H_RES - (jpeg.getWidth() * scale)) / 2;
        int y = (EXAMPLE_LCD_V_RES - (jpeg.getHeight() * scale)) / 2;

        // Decode and display
        jpeg.decode(x, y, scale);
        jpeg.close();
      }
      else
      {
        USBSerial.println("Failed to open JPEG");
      }
    }
    else
    {
      USBSerial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  // Free the buffer if it was allocated
  if (jpegBuffer)
  {
    free(jpegBuffer);
    jpegBuffer = nullptr;
  }
}

void loop()
{
  USBSerial.println(WiFi.localIP());
  delay(1000); // 1 second
}
