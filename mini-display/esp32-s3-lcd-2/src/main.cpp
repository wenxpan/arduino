#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <JPEGDEC.h>
#include <SD.h>
#include <SD_MMC.h>
#include <ESPmDNS.h>

#include "time.h"
#include "./secrets.h"
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"
#include "./display/JpegFunc.h"
#include "./display/GifClass.h"

WebServer server(80);

const int led = 13;

Arduino_GFX *gfx = nullptr;

static GifClass gifClass;

// rotate files config
File root;
String imageFiles[50]; // Array to hold image filenames
int imageCount = 0;
int currentImage = 0;
unsigned long lastChangeTime = 0;
const unsigned long rotationInterval = 5000; // 5 seconds

// Time configuration
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 60 * 60 * 10;
const int daylightOffset_sec = 0;

#define GIF_RAIN "/rain.gif"
#define GIF_MEMORIES "/memories.gif"

void handleRoot()
{
  server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // USBSerial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}
void loadImageList()
{
  root = SD.open("/");
  File file = root.openNextFile();
  imageCount = 0;

  while (file && imageCount < 50)
  {
    if (!file.isDirectory())
    {
      String filename = file.name();
      // Filter hidden files
      // if ((filename.endsWith(".jpg") || filename.endsWith(".jpeg")) &&
      if (filename.endsWith(".gif") &&
          !filename.startsWith("._") &&
          !filename.startsWith("."))
      {
        // Store with full path
        imageFiles[imageCount] = "/" + filename;
        USBSerial.println("Found valid image: " + imageFiles[imageCount]);
        imageCount++;
      }
      else
      {
        USBSerial.println("Skipping: " + filename);
      }
    }
    file = root.openNextFile();
  }

  USBSerial.printf("Found %d valid images\n", imageCount);
}

void setup(void)
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  USBSerial.begin(115200);
  WiFi.softAP(ESP_SSID, ESP_PASS);
  IPAddress myIP = WiFi.softAPIP();
  USBSerial.println("IP: ");
  USBSerial.println(myIP);

  if (MDNS.begin("esp32"))
  {
    USBSerial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/rain", []()
            { drawGifFromSD(gfx, gifClass, GIF_RAIN);
            server.send(200, "text/plain", "rain.gif"); });

  server.on("/memories", []()
            { drawGifFromSD(gfx, gifClass, GIF_MEMORIES);
            server.send(200, "text/plain", "memories.gif"); });

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.onNotFound(handleNotFound);

  server.begin();
  USBSerial.println("HTTP server started");

  // Init Display
  gfx = setupGfx();

  // drawExamplePepper(gfx);
  setupSD();
  logSDInfo();

  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // // WiFi.begin(WIFI_SSID, WIFI_PASS);
  // // while (WiFi.status() != WL_CONNECTED)
  // // {
  // //   delay(500);
  // //   USBSerial.print(".");
  // // }

  // // Load list of JPEG images
  // loadImageList();

  // // Display first image if available
  // if (imageCount > 0)
  // {
  //   // drawJpegFromSD(gfx, imageFiles[currentImage].c_str(), jpegDrawCallback);
  //   drawGifFromSD(gfx, gifClass, imageFiles[currentImage].c_str());
  //   lastChangeTime = millis();
  // }
  // else
  // {
  //   USBSerial.println("No JPEG images found!");
  // }
}

void loop()
{
  server.handleClient();
  delay(2); // allow the cpu to switch to other tasks

  // if (imageCount > 0)
  // {
  //   unsigned long currentTime = millis();

  //   // Check if it's time to change image
  //   if (currentTime - lastChangeTime >= rotationInterval)
  //   {
  //     lastChangeTime = currentTime;

  //     // Move to next image
  //     currentImage = (currentImage + 1) % imageCount;

  //     // Display the image
  //     USBSerial.println("Displaying: " + imageFiles[currentImage]);
  //     // drawJpegFromSD(gfx, imageFiles[currentImage].c_str(), jpegDrawCallback);
  //     drawGifFromSD(gfx, gifClass, imageFiles[currentImage].c_str());
  //   }
  // }

  // delay(1 * 1000);

  // drawGifFromSD(gfx, gifClass, GIF_FILENAME);
}
