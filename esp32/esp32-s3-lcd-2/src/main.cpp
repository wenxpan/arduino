#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <JPEGDEC.h>
#include <SD.h>
#include <SD_MMC.h>

#include "time.h"
#include "./secrets.h"
#include "./display/gfx_setup.h"
#include "./display/draw_utils.h"
#include "./storage/sd_utils.h"
#include "./display/JpegFunc.h"
#include "./display/GifClass.h"

WiFiServer server(80);

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
  USBSerial.begin(115200);
  USBSerial.println("Starting");

  if (!WiFi.softAP(ESP_SSID, ESP_PASS))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  USBSerial.print("AP IP address: ");
  USBSerial.println(myIP);
  server.begin();

  Serial.println("Server started");

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
  WiFiClient client = server.available(); // listen for incoming clients

  if (client)
  {                                // if you get a client,
    Serial.println("New Client."); // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        if (c == '\n')
        { // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<a href=\"/R\">Rain.gif</a><br><br>");
            client.print("<a href=\"/M\">Memories.gif</a><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /R"))
        {
          drawGifFromSD(gfx, gifClass, GIF_RAIN);
        }
        if (currentLine.endsWith("GET /M"))
        {
          drawGifFromSD(gfx, gifClass, GIF_MEMORIES);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
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
