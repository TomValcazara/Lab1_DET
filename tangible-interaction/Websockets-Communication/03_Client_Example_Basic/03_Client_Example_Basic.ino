//###################################################################################################################
//###################################################################################################################
// Extrality Lab 2026 - Stockholm University
// Questions? Contact antonio.braga@dsv.su.se or luis.quintero@dsv.su.se
//###################################################################################################################
// This file should be ran together with the Python server file and one of the ESP32 provided to you at Extrality Lab
// ###################################################################################################################
// To run this file, you need to have the 'Websockets' by Markus Sattler and "Adafruit Neopixel" library installed.
// ###################################################################################################################
// ###################################################################################################################


#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>
#include <header.h>

// --------------------------------------------------
// WiFi / Server Settings
// --------------------------------------------------
const char *ssid = "dsv-extrality-lab";            // Enter your Wi-Fi name
const char *password = "expiring-unstuck-slider";  // Enter Wi-Fi password
const char* serverIP = "10.204.0.43"; //Replace with your Python server's IP (e.g. 192.168.1.111)
const uint16_t serverPort = 8081; //Replace with your desired Port (or keep as is)

// --------------------------------------------------
// Hardware pins
// --------------------------------------------------
#define BUILTIN_BUTTON_PIN 0
#define BTN_RESETGAME 10
#define PIN_WIN_LED 11
#define PIN_POWER 12
// --------------------------------------------------
// WebSocket events setup
// --------------------------------------------------
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

  switch(type) {

  
    case WStype_CONNECTED: {
      Serial.printf("WebSocket connected to %s:%u\n", serverIP, serverPort);
      String message = String("Device: ") + BOARD_NAME + " ... MAC: " + WiFi.macAddress();
      webSocket.sendTXT(message);
      break;
    }

    case WStype_DISCONNECTED:
      Serial.println("Not connected to WebSocket server... Retrying in 5 seconds");
      break;

    case WStype_TEXT:
      payload[length] = '\0';
      Serial.print("WebSocket received: ");
      Serial.println((char*)payload);
      handleMessage(String((char*)payload));
      break;
  }
}

// --------------------------------------------------
// WebSocket message handler. Edit for new RECEIVED WebSocket Messages
// --------------------------------------------------
void handleMessage(const String& message) { // This function is set up to receive and parse messages in the form of "TYPE:VALUE" (e.g. Led:55)
  int sep = message.indexOf(':');
  if (sep == -1) return;

  String type = message.substring(0, sep);
  int value = message.substring(sep + 1).toInt();

  if (type.equalsIgnoreCase("LED_INTENSITY")) { //If this ESP receives a Websocket message of type "LED_INTENSITY", then set the built in LED to the corresponding value
    value = constrain(value, 0, 255);
    ledSet(value);
    Serial.printf("LED intensity → %d\n", value);
  }

  if (type.equalsIgnoreCase("WIN_LED")) {
    if (value == 0) {
      digitalWrite(PIN_WIN_LED, LOW);
    }
    if (value == 1) {
      digitalWrite(PIN_WIN_LED, HIGH);
    }
  }

  if(type.equalsIgnoreCase("CUSTOM WEBSOCKET MESSAGE")) {
    //Do something
  }

}

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("=================================");
  Serial.print("Compiled for board: ");
  Serial.println(BOARD_NAME);
  Serial.println("=================================");

  pinMode(BUILTIN_BUTTON_PIN, INPUT_PULLUP);

  pinMode(BTN_RESETGAME, INPUT_PULLUP);

  pinMode(PIN_WIN_LED, OUTPUT);
  digitalWrite(PIN_WIN_LED, LOW); // start OFF

  #ifdef LED_TYPE_GPIO //If ESP32 regular or S2
    ledcAttach(LED_PIN, LEDC_FREQUENCY, LEDC_RESOLUTION);
    ledcWrite(LED_PIN, 0);
  #endif

  #ifdef LED_TYPE_RGB //If ESP32 S3
    rgbLed.begin();
    rgbLed.clear();
    rgbLed.show();
  #endif

  WiFi.begin(ssid, password);
  Serial.printf("Connecting to WiFi: %s", ssid);
  while (WiFi.status() != WL_CONNECTED) { //Waiting for conection to wifi
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.printf("Attempting to connect to WebSocket Server on %s\n", serverIP);

  webSocket.begin(serverIP, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop() {
  webSocket.loop();

  bool currentButtonState = digitalRead(BUILTIN_BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) { //Sends Websocket Message when you push down the built in button
    webSocket.sendTXT("button:1");
    Serial.println("Button Down");
  }

  if (lastButtonState == LOW && currentButtonState == HIGH) { //Sends Websocket Message when you stop pushing the built in button
    webSocket.sendTXT("button:0");
    Serial.println("Button Up");
  }
  lastButtonState = currentButtonState;



  //Reset Button Logic
  bool currentResetState = digitalRead(BTN_RESETGAME);

  if (lastResetState == HIGH && currentResetState == LOW) { //Sends Websocket Message when you push down the built in button
    webSocket.sendTXT("reset:1");
    Serial.println("Reset Down");
  }

  if (lastResetState == LOW && currentResetState == HIGH) { //Sends Websocket Message when you stop pushing the built in button
    webSocket.sendTXT("reset:0");
    Serial.println("Reset Up");
  }
  lastResetState = currentResetState;


  //Setting power using a potentiometer
  int potValue = analogRead(PIN_POWER); // ESP32: 0–4095
  if (abs(potValue - lastPowerPotValue) > POT_THRESHOLD) {
    lastPowerPotValue = potValue;

    webSocket.sendTXT("FLIPPERPOWER:" + String(potValue));
  }

}










