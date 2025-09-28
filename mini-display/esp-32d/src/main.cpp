#include <Arduino.h>
#include <ezButton.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "./secrets.h"

#define BUTTON_1_PIN 18
#define BUTTON_2_PIN 22
#define LED_PIN 21

ezButton button1(BUTTON_1_PIN);
ezButton button2(BUTTON_2_PIN);
int led_state = LOW;
static bool sendRain = true;

// Async HTTP variables
AsyncClient *client = nullptr;
bool requestPending = false;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  button1.setDebounceTime(50);
  button2.setDebounceTime(50);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ESP_SSID, ESP_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, HIGH);
}

void onData(void *arg, AsyncClient *client, void *data, size_t len)
{
  Serial.printf("Data received: ");
  Serial.write((uint8_t *)data, len);
  Serial.println();
}

void onConnect(void *arg, AsyncClient *client)
{
  Serial.println("Connected to server");

  // Get the path from the argument
  String *path = (String *)arg;

  // Send HTTP GET request with dynamic path
  String request = "GET " + *path + " HTTP/1.1\r\n";
  request += "Host: 192.168.4.1\r\n";
  request += "Connection: close\r\n\r\n";

  Serial.println("Sending request: " + request);
  client->write(request.c_str());
}

void onDisconnect(void *arg, AsyncClient *client)
{
  Serial.println("Disconnected from server");
  requestPending = false;

  // Clean up the allocated path string
  if (arg)
  {
    delete (String *)arg;
  }

  if (client)
  {
    delete client;
    client = nullptr;
  }
}

void onError(void *arg, AsyncClient *client, int error)
{
  Serial.printf("Connection error: %d\n", error);
  requestPending = false;

  // Clean up the allocated path string
  if (arg)
  {
    delete (String *)arg;
  }

  if (client)
  {
    delete client;
    client = nullptr;
  }
}

void onTimeout(void *arg, AsyncClient *client, uint32_t time)
{
  Serial.printf("Connection timeout: %u\n", time);
  requestPending = false;

  // Clean up the allocated path string
  if (arg)
  {
    delete (String *)arg;
  }

  if (client)
  {
    delete client;
    client = nullptr;
  }
}

void sendAsyncRequest(const char *path)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
    return;
  }

  if (requestPending)
  {
    Serial.println("Request already in progress");
    return;
  }

  client = new AsyncClient();
  if (!client)
  {
    Serial.println("Failed to create client");
    return;
  }

  // Allocate a string to hold the path
  String *pathPtr = new String(path);

  requestPending = true;

  client->onData(&onData, pathPtr);
  client->onConnect(&onConnect, pathPtr);
  client->onDisconnect(&onDisconnect, pathPtr);
  client->onError(&onError, pathPtr);
  client->onTimeout(&onTimeout, pathPtr);

  if (!client->connect("192.168.4.1", 80))
  {
    Serial.println("Connect failed");
    delete client;
    delete pathPtr; // Clean up on failure
    client = nullptr;
    requestPending = false;
  }
}

void loop()
{
  button1.loop();
  button2.loop();

  if (button1.isPressed())
  {
    Serial.println("The button1 is pressed");
    Serial.println("Send rain request");
    sendAsyncRequest("/rain");
  }

  if (button2.isPressed())
  {
    Serial.println("The button2 is pressed");
    Serial.println("Send memories request");
    sendAsyncRequest("/memories");
  }

  if (requestPending)
  {
    digitalWrite(LED_PIN, LOW);
  }
  else
  {
    digitalWrite(LED_PIN, HIGH);
  }
}
