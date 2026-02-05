// Pin definitions
//(WARNING: Check your board pinout - Different on ESP32-S3)
const int PIN_POT = A0;    // Potentiometer input pin
const int PIN_LED = 13;    // Onboard LED (GPIO 13, check your board)

// PWM setup
const int LEDC_FRQ = 1000;     // 1 KHz
const int LEDC_BIT = 8;  // 8-bit (0–255)
const int LEDC_CHN = 0;

void setup() {
  pinMode(PIN_POT, INPUT);

  // Force to read analog inputs between 0-4095. Default may be 13 bits resolution
  analogReadResolution(12);

  // Setup PWM channel
  ledcAttachChannel(PIN_LED, LEDC_FRQ, LEDC_BIT, LEDC_CHN);

  Serial.println("Basic 2: ESP32-S2 Analog LED with potentiometer");

  Serial.begin(115200);
}

void loop() {
  int potValue = analogRead(PIN_POT);          // Range depends on board (0-8191 for ESP32-S2)
  int ledValue = map(potValue, 0, 4096, 0, 255); // Map to 8-bit brightness (0-255)

  ledcWrite(PIN_LED, ledValue);            // Set LED brightness

  // Debugging
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print("  LED: ");
  Serial.println(ledValue);

  delay(10);
}
