#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <JPEGDEC.h>
#include <secrets.h>

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

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    EXAMPLE_PIN_NUM_LCD_DC, EXAMPLE_PIN_NUM_LCD_CS,
    EXAMPLE_PIN_NUM_LCD_SCLK, EXAMPLE_PIN_NUM_LCD_MOSI, EXAMPLE_PIN_NUM_LCD_MISO);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus, EXAMPLE_PIN_NUM_LCD_RST, EXAMPLE_LCD_ROTATION, true,
    EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);

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

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  // Initialize display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
    while (1)
      ;
  }
  gfx->fillScreen(RED);

#ifdef EXAMPLE_PIN_NUM_LCD_BL
  pinMode(EXAMPLE_PIN_NUM_LCD_BL, OUTPUT);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_BL, HIGH);
#endif

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

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
        Serial.println("Failed to allocate memory for JPEG buffer");
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

      Serial.printf("Downloaded %d bytes\n", bytesRead);

      // Decode and display the JPEG
      if (jpeg.openRAM(jpegBuffer, jpegBufferSize, JPEGDraw))
      {
        Serial.printf("Image dimensions: %d x %d\n", jpeg.getWidth(), jpeg.getHeight());

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
        Serial.println("Failed to open JPEG");
      }
    }
    else
    {
      Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
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
  // Nothing to do here
  delay(1000);
}