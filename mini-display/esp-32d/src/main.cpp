#include <Arduino.h>
#include <ezButton.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "./secrets.h"

#define BUTTON_PIN 18
#define LED_PIN 21

ezButton button(BUTTON_PIN);

int led_state = LOW;
bool shouldSendRequest = false;
unsigned long requestStartTime = 0;
bool requestInProgress = false;
static bool sendRain = true;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  button.setDebounceTime(50);

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
}

void sendRequest(const char *path)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String url = String("http://192.168.4.1") + path;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      Serial.printf("GET %s response: %d\n", path, httpCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.printf("GET %s failed, error: %s\n", path, http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}

void loop()
{
  button.loop();
  if (button.isPressed() && !requestInProgress)
  {
    Serial.println("The button is pressed");
    led_state = !led_state;
    Serial.println(led_state);
    digitalWrite(LED_PIN, led_state);

    shouldSendRequest = true;
  }

  if (shouldSendRequest && !requestInProgress)
  {
    if (sendRain)
    {
      Serial.println("Send rain request");
      sendRequest("/rain");
    }
    else
    {
      Serial.println("Send memories request");
      sendRequest("/memories");
    }
    sendRain = !sendRain;
    shouldSendRequest = false;
    requestInProgress = true;
    requestStartTime = millis();
  }

  if (requestInProgress && (millis() - requestStartTime > 1000))
  {
    requestInProgress = false;
  }
}
