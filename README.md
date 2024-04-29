# ESP32-WROOM-32 with round 240x240 TFT using the SPI ST7789 interface controller

- Example project to setup the round 240x240 round TFT screen connected to an ESP32-WROOM-32 using PlatformIO IDE for VSCode.
- I'm using the bodmer/TFT_eSPI display driver library.
- The platformio.ini file has the screen size and pins set for this particular screen.
- Some manual change is required inside the TFT_eSPI library user setup. See below

# This uses the round 1.28" 240 240 TFT screen.
this is done by setting as similarly but changing the platformio.ini file to include `-D GC9A01_DRIVER=1` and changing `.pio/libdeps/upesy_wroom/TFT_eSPI/User_Setup.h`  to select `GC9A01_DRIVER` (instead of `ILI9341_DRIVER`)



# Uploading
its no longer needed that you convert jpg files for display with tft. they can be supplied via the file system as raw jpgs and decoding can take place on board. Eg:
- fieldfare.jpg `data/fieldfare.jpg`
this file is included using `SPIFFS` flash memory system. to use this, define the filesystem type in platformio.ini file (`board_build.filesystem = spiffs`) and then (after flashing) upload the files via the command line:
- `pio run -t uploadfs`
the file can then be accessed via SPIFFS using `TJpgDec.getFsJpgSize(&w, &h, "/fieldfare.jpg");`