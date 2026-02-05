// --------------------------------------------------
// Automatic board detection and built in LED setup
// --------------------------------------------------

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
  #define LED_COUNT 1

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

// --------------------------------------------------
// Global variables
// --------------------------------------------------
WebSocketsClient webSocket;
bool lastButtonState = HIGH;
int lastPotValue = -1;
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL_MS = 50;


bool lastResetState = HIGH;
int lastPowerPotValue = -1;
const int POT_THRESHOLD = 100; // noise filter


// --------------------------------------------------
// LEDC (PWM) settings
// --------------------------------------------------
#define LEDC_FREQUENCY 5000
#define LEDC_RESOLUTION 8  // 0–255

// --------------------------------------------------
// RGB LED object (ESP32-S3 only)
// --------------------------------------------------
#ifdef LED_TYPE_RGB
  Adafruit_NeoPixel rgbLed(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif


// --------------------------------------------------
// Built-in LED functions
// --------------------------------------------------
void ledSet(uint8_t brightness) {
  #ifdef LED_TYPE_GPIO //ESP32 regular and S2 boards
    ledcWrite(LED_PIN, brightness);
  #endif

  #ifdef LED_TYPE_RGB //ESP32 S3 Boards
    rgbLed.setBrightness(brightness);
    rgbLed.setPixelColor(0, rgbLed.Color(255, 255, 255));
    rgbLed.show();
  #endif
}

  void ledOn()  { ledSet(255); }
  void ledOff() { ledSet(0);   }