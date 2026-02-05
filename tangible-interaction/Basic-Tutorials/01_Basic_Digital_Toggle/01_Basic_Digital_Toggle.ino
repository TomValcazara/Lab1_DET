// Pin definitions
//(WARNING: Check your board pinout - Different on ESP32-S3)
const int PIN_LED = 13;   // Onboard LED (often GPIO 13)
const int PIN_BTN = 0;    // Boot button (often GPIO 0)

// Variable to store LED state
bool ledState = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP); // Button is usually active LOW
  digitalWrite(PIN_LED, LOW);

  Serial.begin(115200);
  Serial.println("Basic 1: ESP32-S2 LED toggle with button");
}

void loop() {
  static bool lastBtnState = HIGH;
  bool currentBtnState = digitalRead(PIN_BTN);

  // Detect button press (active LOW, so check for falling edge)
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    ledState = !ledState;             // Toggle state
    digitalWrite(PIN_LED, ledState);  // Apply new state
    Serial.println(ledState ? "LED: ON" : "LED: OFF");
    delay(200); // simple debounce
  }

  lastBtnState = currentBtnState;
}
