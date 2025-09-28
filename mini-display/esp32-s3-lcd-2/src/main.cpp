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

// GIF management variables
String gifFiles[50]; // Array to hold GIF filenames
int gifCount = 0;
int currentGifIndex = 0;
bool shouldStopCurrentGif = false;
bool isPlayingGif = false;
String currentGifFile = "";

// Time configuration
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 60 * 60 * 10;
const int daylightOffset_sec = 0;

void handleRoot()
{
  server.send(200, "text/plain", "hello from esp32!");
}

void handleNext()
{
  USBSerial.println("Handling next");
  USBSerial.println(gifCount);

  if (gifCount > 0)
  {
    shouldStopCurrentGif = true;
    currentGifIndex = (currentGifIndex + 1) % gifCount;
    USBSerial.printf("Next GIF: %s (index %d)\n", gifFiles[currentGifIndex].c_str(), currentGifIndex);

    // Wait a moment for current GIF to stop
    delay(100);

    drawGifFromSD(gfx, gifClass, gifFiles[currentGifIndex].c_str());
    server.send(200, "text/plain", "Next: " + gifFiles[currentGifIndex]);
  }
  else
  {
    server.send(404, "text/plain", "No GIFs available");
  }
}

void handlePrev()
{
  if (gifCount > 0)
  {
    shouldStopCurrentGif = true;
    currentGifIndex = (currentGifIndex - 1 + gifCount) % gifCount;
    USBSerial.printf("Previous GIF: %s (index %d)\n", gifFiles[currentGifIndex].c_str(), currentGifIndex);

    // Wait a moment for current GIF to stop
    delay(100);

    drawGifFromSD(gfx, gifClass, gifFiles[currentGifIndex].c_str());
    server.send(200, "text/plain", "Previous: " + gifFiles[currentGifIndex]);
  }
  else
  {
    server.send(404, "text/plain", "No GIFs available");
  }
}

void handleSpecificGif()
{
  String uri = server.uri();
  String gifName = uri.substring(1) + ".gif"; // Remove leading '/' and add .gif extension
  String fullPath = "/" + gifName;

  USBSerial.printf("Requested GIF: %s\n", fullPath.c_str());

  if (SD.exists(fullPath))
  {
    shouldStopCurrentGif = true;

    // Update current index if this GIF is in our list
    for (int i = 0; i < gifCount; i++)
    {
      if (gifFiles[i] == fullPath)
      {
        currentGifIndex = i;
        break;
      }
    }

    // Wait a moment for current GIF to stop
    delay(100);

    drawGifFromSD(gfx, gifClass, fullPath.c_str());
    server.send(200, "text/plain", "Playing: " + gifName);
  }
  else
  {
    server.send(404, "text/plain", "GIF not found: " + gifName);
  }
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

  // Add available GIFs to the response
  message += "\nAvailable GIFs:\n";
  for (int i = 0; i < gifCount; i++)
  {
    String gifName = gifFiles[i];
    gifName.replace("/", "");
    gifName.replace(".gif", "");
    message += "  /" + gifName + "\n";
  }
  message += "  /next\n";
  message += "  /prev\n";

  server.send(404, "text/plain", message);
}

void loadGifList()
{
  File root = SD.open("/");
  File file = root.openNextFile();
  gifCount = 0;

  USBSerial.println("Loading GIF list...");

  while (file && gifCount < 50)
  {
    if (!file.isDirectory())
    {
      String filename = file.name();
      // Filter for GIF files only, exclude hidden files
      if (filename.endsWith(".gif") &&
          !filename.startsWith("._") &&
          !filename.startsWith("."))
      {
        // Store with full path
        gifFiles[gifCount] = "/" + filename;
        USBSerial.println("Found GIF: " + gifFiles[gifCount]);
        gifCount++;
      }
    }
    file = root.openNextFile();
  }

  USBSerial.printf("Found %d GIF files\n", gifCount);

  if (gifCount > 0)
  {
    currentGifIndex = 0;
    USBSerial.printf("Default GIF: %s\n", gifFiles[currentGifIndex].c_str());
  }
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

  // Init Display
  gfx = setupGfx();

  // Setup SD card and load GIF list
  setupSD();
  logSDInfo();
  loadGifList();

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/next", handleNext);
  server.on("/prev", handlePrev);

  // Handle specific GIF requests (like /rain, /memories, etc.)
  server.onNotFound([]()
                    {
    String uri = server.uri();
    if (uri != "/" && uri.length() > 1)
    {
      // Try to handle as GIF request first
      String gifName = uri.substring(1) + ".gif";
      String fullPath = "/" + gifName;
      
      if (SD.exists(fullPath))
      {
        handleSpecificGif();
        return;
      }
    }
    // If not a valid GIF request, show 404
    handleNotFound(); });

  server.begin();
  USBSerial.println("HTTP server started");

  // Start with first GIF if available
  if (gifCount > 0)
  {
    USBSerial.println("Starting with first GIF...");
    drawGifFromSD(gfx, gifClass, gifFiles[currentGifIndex].c_str());
  }
}

void loop()
{
  server.handleClient();
  delay(2);
}
