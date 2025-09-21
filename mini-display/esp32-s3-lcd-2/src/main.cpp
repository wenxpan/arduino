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

// Rotate files config (unchanged)
File root;
String imageFiles[50]; // Array to hold image filenames
int imageCount = 0;
int currentImage = 0;
unsigned long lastChangeTime = 0;
const unsigned long rotationInterval = 5000; // 5 seconds

// Time configuration (unchanged)
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 60 * 60 * 10;
const int daylightOffset_sec = 0;

#define GIF_RAIN "/rain.gif"
#define GIF_MEMORIES "/memories.gif"

String getContentType(String filename)
{
  if (filename.endsWith(".html"))
    return "text/html";
  if (filename.endsWith(".htm"))
    return "text/html";
  if (filename.endsWith(".js"))
    return "application/javascript";
  if (filename.endsWith(".css"))
    return "text/css";
  if (filename.endsWith(".json"))
    return "application/json";
  if (filename.endsWith(".png"))
    return "image/png";
  if (filename.endsWith(".jpg"))
    return "image/jpeg";
  if (filename.endsWith(".jpeg"))
    return "image/jpeg";
  if (filename.endsWith(".gif"))
    return "image/gif";
  if (filename.endsWith(".svg"))
    return "image/svg+xml";
  if (filename.endsWith(".ico"))
    return "image/x-icon";
  // Add more as needed
  return "application/octet-stream"; // Default for unknown types
}

void handleRoot()
{
  String path = "/www" + server.uri(); // Assuming your files are in /www on SD card
  if (path.endsWith("/"))
    path += "index.html"; // Handle directories by defaulting to index.html

  if (SD.exists(path))
  {
    File file = SD.open(path, FILE_READ);
    if (file)
    {
      String contentType = getContentType(path);
      server.streamFile(file, contentType);
      file.close();
      return; // File served successfully
    }
  }

  // If the file doesn't exist, serve index.html for SPA routing
  String indexPath = "/www/index.html";
  if (SD.exists(indexPath))
  {
    File file = SD.open(indexPath, FILE_READ);
    if (file)
    {
      server.streamFile(file, "text/html");
      file.close();
      return; // Serve index.html
    }
  }
}

void handleNotFound()
{
  // If nothing is found, send a real 404
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

// pixel drawing callback (unchanged)
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
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
            { drawGifFromSD(gfx, gifClass, GIF_RAIN); });

  server.on("/memories", []()
            { drawGifFromSD(gfx, gifClass, GIF_MEMORIES); });

  server.onNotFound(handleNotFound);

  server.begin();
  USBSerial.println("HTTP server started");

  gfx = setupGfx();
  setupSD();
  logSDInfo();
}

void loop()
{
  server.handleClient();
  delay(2); // Allow CPU to switch tasks
}
