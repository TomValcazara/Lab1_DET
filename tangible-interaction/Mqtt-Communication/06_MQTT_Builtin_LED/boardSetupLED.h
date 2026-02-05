/***
Project: boardSetupLED.h
Author: Luis Quintero
Date: 2026-02-02
Board: All boards -
Description:
Basic sketch configuring the built-in LED pin 
to prototypie an Arduino sketch on any ESP32

Behavior:
- Defines LED_PIN with the pin where the built-in LED
is located
- Defines LED_TYPE_GPIO if it's a normal LED.
- Defines LED_TYPE_RGB if it's an RGB LED
- Defines BOARD_NAME with the string of the board

***/

////////////////////////////////////
//////////// BOARD DETECTION AND 
//////////// BUILT-IN LED SETUP
////////////////////////////////////

#if defined(CONFIG_IDF_TARGET_ESP32)
  #define BOARD_NAME "SparkFun ESP32 Thing Plus"
  #define LED_TYPE_GPIO
  #define LED_PIN 13

#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  #define BOARD_NAME "SparkFun ESP32-S2 Thing Plus"
  #define LED_TYPE_GPIO
  #define LED_PIN 13

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define LED_TYPE_RGB

  #if defined(ARDUINO_WAVESHARE_ESP32_S3_ZERO)
    #define BOARD_NAME "Waveshare ESP32-S3 Zero"
    #define LED_PIN 21
  #else
    #define BOARD_NAME "ESP32-S3 DevKit"
    #define LED_PIN 38
  #endif
#else
  #error "Unsupported ESP32 board... Check if you've selected the correct board before uploading code"
#endif