# ESP32-WROOM-32 with round 240x280 TFT using the SPI ST7789 interface controller

- Example project to setup the round 240x240 round TFT screen connected to an ESP32-WROOM-32 using PlatformIO IDE for VSCode.
- I'm using the bodmer/TFT_eSPI display driver library.
- The platformio.ini file has the screen size and pins set for this particular screen.
- Some manual change is required inside the TFT_eSPI library user setup. See below

# This uses the round 1.28" 240 240 TFT screen.
this is done by setting as similarly but changing the platformio.ini file to include `-D GC9A01_DRIVER=1` and changing `.pio/libdeps/upesy_wroom/TFT_eSPI/User_Setup.h`  to select `GC9A01_DRIVER` (instead of `ILI9341_DRIVER`)