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
#include <TJpg_Decoder.h>

TFT_eSPI tft = TFT_eSPI(); // TFT object

TFT_eSprite spr = TFT_eSprite(&tft); // Sprite object

// something
#include "bird.h"
#include "info.h"

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
}

// =======================================================================================
// Loop
// =======================================================================================

void loop() {
  Serial.println("loop");

  // Common_Blackbird
  // tft.pushImage(0, 0, 240, 240, (uint16_t *)info, 1);

  // int xw = tft.width() / 2; // xw, yh is middle of screen
  // int yh = tft.height() / 2;
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(100, 100, infoWidth, infoHeight, info);
  showMessage("info icon");
  delay(2000);

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getJpgSize(&w, &h, Common_Blackbird, sizeof(Common_Blackbird));
  Serial.print("Width = ");
  Serial.print(w);
  Serial.print(", height = ");
  Serial.println(h);
  TJpgDec.drawJpg(0, 0, Common_Blackbird, sizeof(Common_Blackbird));
  showMessage("common blackbird");

  // tft.pushImage(0, 0, COMMON_BLACKBIRD_WIDTH, COMMON_BLACKBIRD_HEIGHT,
  //               Common_Blackbird);

  // tft.setPivot(xw, yh); // Set pivot to middle of TFT screen
  // drawX(xw, yh);        // Show where screen pivot is

  // // Create the Sprite
  // spr.setColorDepth(8);      // Create an 8bpp Sprite of 60x30 pixels
  // spr.createSprite(64, 30);  // 8bpp requires 64 * 30 = 1920 bytes
  // spr.setPivot(32, 55);      // Set pivot relative to top left corner of
  // Sprite spr.fillSprite(TFT_BLACK); // Fill the Sprite with black

  // spr.setTextColor(TFT_GREEN);        // Green text
  // spr.setTextDatum(MC_DATUM);         // Middle centre datum
  // spr.drawString("Hello", 32, 15, 4); // Plot text, font 4, in Sprite at 30,
  // 15

  // spr.pushRotated(0);
  // spr.pushRotated(90);
  // spr.pushRotated(180);
  // spr.pushRotated(270);

  // delay(2000);

  // showMessage("45 degree angles");
  // drawX(xw, yh); // Show where screen pivot is

  // spr.pushRotated(45);
  // spr.pushRotated(135);
  // spr.pushRotated(225);
  // spr.pushRotated(315);

  // delay(2000); // Pause so we see it

  // showMessage("Moved Sprite pivot point");
  // drawX(xw, yh); // Show where screen pivot is

  // spr.setPivot(-20, 15); // Change just the Sprite pivot point

  // spr.pushRotated(45);
  // spr.pushRotated(135);
  // spr.pushRotated(225);
  // spr.pushRotated(315);

  // delay(2000); // Pause so we see it

  // showMessage("Moved TFT pivot point");
  // tft.setPivot(100, 100); // Change just the TFT pivot point
  // drawX(100, 100);        // Show where pivot is

  // spr.pushRotated(45);
  // spr.pushRotated(135);
  // spr.pushRotated(225);
  // spr.pushRotated(315);

  // delay(2000); // Pause so we see it

  // showMessage("Transparent rotations");
  // tft.fillCircle(xw, yh, 70, TFT_DARKGREY); // Draw a filled circle

  // tft.setPivot(xw, yh); // Set pivot to middle of screen
  // drawX(xw, yh);        // Show where pivot is

  // spr.deleteSprite();

  // spr.setColorDepth(8);     // Create a 8bpp Sprite
  // spr.createSprite(40, 30); // Create a new Sprite 40x30
  // spr.setPivot(20, 70);     // Set Sprite pivot at 20,80

  // spr.setTextColor(TFT_RED);  // Red text in Sprite
  // spr.setTextDatum(MC_DATUM); // Middle centre datum

  // int num = 1;

  // for (int16_t angle = 30; angle <= 360; angle += 30) {
  //   spr.fillSprite(TFT_BLACK); // Clear the Sprite
  //   spr.drawNumber(num, 20, 15,
  //                  4); // Plot number, in Sprite at 20,15 and with font 4
  //   spr.pushRotated(angle,
  //                   TFT_BLACK); // Plot rotated Sprite, black being
  //                   transparent
  //   num++;
  // }

  // spr.setTextColor(TFT_WHITE); // White text in Sprite
  // spr.setPivot(-75, 15);       // Set Sprite pivot at -75,15

  // for (int16_t angle = -90; angle < 270; angle += 30) {
  //   spr.fillSprite(TFT_BLACK); // Clear the Sprite
  //   spr.drawNumber(angle + 90, 20, 15,
  //                  4); // Plot number, in Sprite at 20,15 and with font 4
  //   spr.pushRotated(angle,
  //                   TFT_BLACK); // Plot rotated Sprite, black being
  //                   transparent
  //   num++;
  // }

  delay(8000); // Pause so we see it

  spr.deleteSprite();
}