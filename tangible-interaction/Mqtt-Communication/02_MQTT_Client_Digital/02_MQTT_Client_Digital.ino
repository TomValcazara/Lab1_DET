/*
  Project: 02_MQTT_Client_Digital
  Author: Luis Quintero
  Date: 2025-10-03
  Board: ESP32-S2 (Adapt Pin Layout for ESP32 or ESP32-S3)
  Description:
    Basic sketch demonstrating DIGITAL inputs and outputs 
    with MQTT. 
    
    Behavior:
    - The built-in button toggles the built-in LED.
    
    All communication goes through MQTT Messages. Note that
    the WiFi and MQTT values default to the credentials
    required to connect with the MQTT Broker at Extrality Lab.
*/

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi setup - Default setup to connect to Extrality Lab WiFi
const char *wifi_ssid = "dsv-extrality-lab";            // Enter your Wi-Fi name
const char *wifi_password = "expiring-unstuck-slider";  // Enter Wi-Fi password

WiFiClient espClient; // variable in charge of WiFi connection

// MQTT Broker setup - Default setup to connect to ExtralityLab Broker
const char *mqtt_broker = "inf-liva-server-eth.dsv.local.su.se";
const int mqtt_port = 1883;
const char *mqtt_username = "esp32";
const char *mqtt_password = "extrality";

PubSubClient client(espClient); // variable in charge of Mqtt connection
String client_id = "";

////////////////////////////////////
//////////// PIN DEFINITION
////////////////////////////////////

// PIN INPUTS
const int PIN_BTN = 0;    //Built in button
bool boardBtnState;     // variable to store current button state
bool lastBoardBtnState = HIGH;  // using INPUT_PULLUP: HIGH = not pressed (pullup), LOW = pressed

// PIN OUTPUT LED CONTROL
const int PIN_LED_DIG = 13;  // Onboard LED Digital (GPIO 13 in SparkFun ESP32S2 board)
bool ledDigitalState = LOW;


////////////////////////////////////
//////////// MQTT SETUP
////////////////////////////////////

//// Topics (Inputs sent to MQTT Broker)
// Digital
char topicBoardButton[50] = "/D0_BoardBtn";

//// Subscribers (Actuators received from MQTT Broker)
//Digital
char subsD13_out_BoardLED[50] = "/D13_boardLED";

////////////////////////////////////
//////////// CUSTOM FUNCTIONS
////////////////////////////////////


String getMacAddress() {
  // Get the MAC address of the ESP32
  String macStr = String(WiFi.macAddress());  // Format: "AA:BB:CC:DD:EE:FF"
  Serial.print("Full MAC address:");
  Serial.println(macStr);

  // Optional: Remove colons if you want a clean topic name
  macStr.replace(":", "");
  String macStr4 = macStr.substring(macStr.length() - 4);
  macStr4.toLowerCase();

  Serial.print("Device ID (Last four characters): ");
  Serial.println(macStr4);
  return macStr4;
}


bool ensureMqtt() {
  if (client.connected()) return true;
  while (!client.connected()) {
    Serial.printf("The client %s connects to the ExtralityLab MQTT broker\n", client_id.c_str());
    bool isConnected = client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    if (isConnected) {
      Serial.println("ExtralityLab MQTT broker connected");
      return true;
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


// Notify that there is a function called `mqttCallback`. Implementation is further below.
void mqttCallback(char *subscriber, byte *payload, unsigned int length);

////////////////////////////////////
//////////// SETUP
////////////////////////////////////

void setup() {

  // Set software serial baud to 115200;
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  delay(100);

  //// Set up pinModes
  // Inputs
  pinMode(PIN_BTN, INPUT_PULLUP); // Built-in button

  // Outputs/Actuators
  pinMode(PIN_LED_DIG, OUTPUT); // Built-in LED as Digital
  digitalWrite(PIN_LED_DIG, LOW);

  ///////////////////
  // Connecting to a WiFi network
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wi-Fi network");


  ///////////////////
  // Connecting to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqttCallback); // What would happen with each MQTT message

  // Get MAC Address to create client ID
  String deviceMacID = getMacAddress();
  client_id = "esp32-client-";
  client_id += deviceMacID; // unique clientID based on MAC address

  bool connectedMqtt = ensureMqtt();

  ///////////////////
  // Publish Topics (one per input)
  Serial.println("\t[[MQTT Published Topics]]");

  // Build the topics strings, preceding the client_id
  String topic;

  topic = deviceMacID + topicBoardButton;
  topic.toCharArray(topicBoardButton, sizeof(topicBoardButton));
  Serial.println(topicBoardButton);

  ///////////////////
  ///// Subscribe to topics (one per actuator)
  Serial.println("\t[[MQTT subscriptions]]");

  // Build the subs strings, preceding the client_id
  String subs;

  subs = deviceMacID + subsD13_out_BoardLED;
  subs.toCharArray(subsD13_out_BoardLED, sizeof(subsD13_out_BoardLED));
  client.subscribe(subsD13_out_BoardLED);
  Serial.println(subsD13_out_BoardLED);
}

////////////////////////////////////
//////////// MAIN LOOP
////////////////////////////////////

void loop() {

  // Ensure connections
  ensureMqtt();
  client.loop();

  // ---- topicBoardButton: Publish on change ---
  boardBtnState = digitalRead(PIN_BTN);
  if (boardBtnState != lastBoardBtnState) {
    lastBoardBtnState = boardBtnState;

    client.publish(topicBoardButton, String(boardBtnState).c_str());
    Serial.print("Board button: ");
    Serial.println(boardBtnState);

    ///////////
    //// EDIT: Here we manually also announce the topics of the actuator
    // that we would like to control with the input
    client.publish(subsD13_out_BoardLED, "toggle");
    ///////////
  }

  delay(5); // small idle

}

////////////////////////////////////
//////////// MQTT SUBSCRIPTION CALLBACKS
////////////////////////////////////

void mqttCallback(char *subscriber, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in Subscriber: ");
  Serial.println(subscriber);
  Serial.print("Message:");

  String message;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println("-----------------------");

  ///// Select actuator based on subscriber's name:

  // ---- subsD13_out_BoardLED: Built-in Board LED as Digital---
  if (strcmp(subscriber, subsD13_out_BoardLED) == 0)
  {
    Serial.println("Built in Led...");
    if (message == "true") {
      digitalWrite(PIN_LED_DIG, true);
    } else if (message == "false") {
      digitalWrite(PIN_LED_DIG, false);
    } else if (message == "toggle") {
      digitalWrite(PIN_LED_DIG, !digitalRead(PIN_LED_DIG));
    } else {
      Serial.println("Error! Unknown message. Valid messages: ['true','false','toggle']");
    }
  } else {
    Serial.println("Error! Unknown subscriber.");
  }
}