
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include <config.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include <esp_now.h>

#define PIN 4
#define NUMPIXELS 88
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 800 // Time (in milliseconds) to pause between pixels
int pixelIndex = 0;
int timeOfMessage = 0;
// uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0x47, 0x42, 0x00};

typedef struct light_message
{
  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
} light_message;

light_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  digitalWrite(32, HIGH);
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(myData.Red, myData.Green, myData.Blue));
  }
  pixels.show();
  timeOfMessage = millis();
}

void setup()
{
  pinMode(32, OUTPUT);
  Serial.begin(115200);
  Serial.println("Booting");

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x66};

  pixels.begin();
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    pixels.setPixelColor(2, pixels.Color(255, 50, 0));
    pixels.setPixelColor(1, pixels.Color(255, 50, 0));
    pixels.setPixelColor(0, pixels.Color(255, 50, 0));
    pixels.show();
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("SPLight");

  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

  // uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x66};
  // esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  Serial.println("Ready");
  Serial.print("IP address: ");
  pinMode(35, INPUT);
  if (digitalRead(35))
  {
    Serial.println(esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]), HEX);
    Serial.println(WiFi.macAddress());
  }

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  pixels.clear();
  for (int i = 2; i < NUMPIXELS - 2; i++)
  {
    pixels.clear();
    pixels.setPixelColor(i - 2, pixels.Color(10, 10, 10));
    pixels.setPixelColor(i - 1, pixels.Color(50, 50, 50));
    pixels.setPixelColor(i, pixels.Color(100, 100, 100));
    pixels.setPixelColor(i - 1, pixels.Color(50, 50, 50));
    pixels.setPixelColor(i + 2, pixels.Color(10, 10, 10));
    pixels.show();
    delay(50);
  }
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(100, 100, 100));
    pixels.show();
    delay(10);
  }
}

void loop()
{
  // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  // For each pixel...

  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  // Here we're using a moderately bright green color:

  // pixels.clear();

  if (millis() - timeOfMessage > 500)
  {
    digitalWrite(32, LOW);
  }
  ArduinoOTA.handle();
}
