// This example plots a rotated Sprite to the screen using the pushRotated()
// function. It is written for a 240 x 320 TFT screen.

// Two rotation pivot points must be set, one for the Sprite and one for the TFT
// using setPivot(). These pivot points do not need to be within the visible
// screen or Sprite boundary.

// When the Sprite is rotated and pushed to the TFT with pushRotated(angle) it
// will be drawn so that the two pivot points coincide. This makes rotation
// about a point on the screen very simple. The rotation is clockwise with
// increasing angle. The angle is in degrees, an angle of 0 means no Sprite
// rotation.

// The pushRotated() function works with 1, 4, 8 and 16 bit per pixel (bpp)
// Sprites.

// The original Sprite is unchanged so can be plotted again at a different
// angle.

// Optionally a transparent colour can be defined, pixels of this colour will
// not be plotted to the TFT.

// For 1 bpp Sprites the foreground and background colours are defined with the
// function spr.setBitmapColor(foregroundColor, backgroundColor).

// For 4 bpp Sprites the colour map index is used instead of the 16 bit colour
// e.g. spr.setTextColor(5); // Green text in default colour map
// See "Transparent_Sprite_Demo_4bit" example for default colour map details

// Created by Bodmer 6/1/19 as an example to the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

#include <TFT_eSPI.h>
#include <stdint.h>
// Include the jpeg decoder library
#include "secrets.h"
// jpg decoding library
#include <TJpg_Decoder.h>
// wifi mangager
#include <WiFi.h>
// flash memory filesystem
#include "SPIFFS.h"
// http client
#include "HTTPClient.h"

TFT_eSPI tft = TFT_eSPI(); // TFT object

TFT_eSprite spr = TFT_eSprite(&tft); // Sprite object

// something
// #include "common_blackbird.h"
// #include "fieldfare.h"

// #include "info.h"

// =======================================================================================
// Draw an X centered on x,y
// =======================================================================================

void drawX(int x, int y) {
  tft.drawLine(x - 5, y - 5, x + 5, y + 5, TFT_WHITE);
  tft.drawLine(x - 5, y + 5, x + 5, y - 5, TFT_WHITE);
}

// =======================================================================================
// Show a message at the top of the screen
// =======================================================================================

void showMessage(String msg) {
  // Clear the screen areas
  tft.fillRect(0, 0, tft.width(), 20, TFT_BLACK);
  // tft.fillRect(0, 20, tft.width(), tft.height() - 20, TFT_BLUE);

  uint8_t td = tft.getTextDatum(); // Get current datum

  tft.setTextDatum(TC_DATUM); // Set new datum

  tft.drawString(msg, tft.width() / 2, 2, 2); // Message in font 2

  tft.setTextDatum(td); // Restore old datum
}

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h,
                uint16_t *bitmap) {
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height())
    return 0;

  // This function will clip the image block rendering automatically at the TFT
  // boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX
  // library tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

// =======================================================================================
// Setup
// =======================================================================================

void setup() {
  Serial.begin(115200); // Debug only

  tft.begin(); // initialize

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  Serial.println("TFT_eSprite demo");

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }

  // connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

// Fetch a file from the URL given and save it in SPIFFS
// Return 1 if a web fetch was needed or 0 if file already exists
bool getFile(String url, String filename) {

  // If it exists then no need to fetch it
  if (SPIFFS.exists(filename) == true) {
    Serial.println("Found " + filename);
    return 0;
  }

  Serial.println("Downloading " + filename + " from " + url);

  // Check WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    Serial.print("[HTTP] begin...\n");

#ifdef ARDUINO_ARCH_ESP8266
    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    client->setInsecure();
    HTTPClient http;
    http.begin(*client, url);
#else
    HTTPClient http;
    // Configure server and url
    http.begin(url);
#endif

    Serial.print("[HTTP] GET...\n");
    // Start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      fs::File f = SPIFFS.open(filename, "w+");
      if (!f) {
        Serial.println("file open failed");
        return 0;
      }
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // File found at server
      if (httpCode == HTTP_CODE_OK) {

        // Get length of document (is -1 when Server sends no Content-Length
        // header)
        int total = http.getSize();
        int len = total;

        // Create buffer for read
        uint8_t buff[128] = {0};

        // Get tcp stream
        WiFiClient *stream = http.getStreamPtr();

        // Read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          // Get available data size
          size_t size = stream->available();

          if (size) {
            // Read up to 128 bytes
            int c = stream->readBytes(
                buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // Write it to file
            f.write(buff, c);

            // Calculate remaining bytes
            if (len > 0) {
              len -= c;
            }
          }
          yield();
        }
        Serial.println();
        Serial.print("[HTTP] connection closed or file end.\n");
      }
      f.close();
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return 1; // File was fetched from web
}

String url = "http://192.168.1.185:8080/captured.jpg";

// Server and image details
const char *server = "192.168.1.185:8080";
const char *imagePath = "/captured.jpg";

// =======================================================================================
// Loop
// =======================================================================================
int i = 0;
void loop() {
  Serial.println("loop:" + i);
  delay(1000);

  tft.fillScreen(TFT_RED);

  // Time recorded for test purposes
  uint32_t t = millis();

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getFsJpgSize(&w, &h, "/fieldfare.jpg"); // Note name preceded with "/"
  Serial.print("Width = ");
  Serial.print(w);
  Serial.print(", height = ");
  Serial.println(h);

  // Draw the image, top left at 0,0
  TJpgDec.drawFsJpg(0, 0, "/fieldfare.jpg");

  // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32
  // SPI 120ms, 8bit parallel 105ms
  t = millis() - t;
  Serial.print(t);
  Serial.println(" ms");

  // Wait before drawing again
  delay(2000);

  delay(1000);
  spr.deleteSprite();
  i++;

  // laoding images
  // tft.fillScreen(TFT_BLACK);
  // tft.pushImage(100, 100, infoWidth, infoHeight, info);
  // showMessage("info icon");
  // delay(2000);
  // TJpgDec.drawJpg(0, 0, Common_Blackbird, sizeof(Common_Blackbird));
  // showMessage("common blackbird");
  // delay(4000);
  // TJpgDec.drawJpg(0, 0, Fieldfare, sizeof(Fieldfare));
  // showMessage("fieldfare");
  // delay(4000);
}