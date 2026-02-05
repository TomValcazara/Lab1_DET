/*
Project: 06_MQTT_AllBoards_Light
Author: Luis Quintero
Date: 2026-02-02
Board: All boards -
Description:
Basic sketch demonstrating a DIGITAL inputs and 
an ANALOG output with MQTT, compatible with many ESP32 boards
Some boards have a red led (ESP32-S2), some boards have a
built-in *RGB* light (ESP32-S3), the code configures
the functions depending on board.

Behavior:
- The built-in button sends (publishes) an MQTT
message when pressed.
- The build-in LED receives (subscribes) an MQTT
message to be turned-on.

All communication goes through MQTT Messages. Note that
the WiFi and MQTT values default to the credentials
required to connect with the MQTT Broker at Extrality Lab.
*/

#include <boardSetupLED.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi setup - Default setup to connect to Extrality Lab WiFi
const char *wifi_ssid = "dsv-extrality-lab";           // Enter your Wi-Fi name
const char *wifi_password = "expiring-unstuck-slider"; // Enter Wi-Fi password

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
const int PIN_BTN = 0;         // Built in button
bool boardBtnState;            // variable to store current button state
bool lastBoardBtnState = HIGH; // using INPUT_PULLUP: HIGH = not pressed (pullup), LOW = pressed

// PIN OUTPUT LED CONTROL
const int PIN_LED_BOARD = LED_PIN; // Onboard LED configured in file `boardSetupLED.h`
bool lastLedState = true;
const int LEDC_FRQ = 1000; // 1 KHz
const int LEDC_BIT = 8;    // 8-bit (0–255)
const int LEDC_CHN = 0;    // PWM Channel

////////////////////////////////////
//////////// MQTT SETUP
////////////////////////////////////

//// Topics (Inputs sent to MQTT Broker)
// Digital
char topicBoardButton[50] = "/boardBTN";

//// Subscribers (Actuators received from MQTT Broker)
// Digital
char subs_out_BoardLED[50] = "/boardLED";

////////////////////////////////////
//////////// CUSTOM FUNCTIONS
////////////////////////////////////

String getMacAddress()
{
    // Get the MAC address of the ESP32
    String macStr = String(WiFi.macAddress()); // Format: "AA:BB:CC:DD:EE:FF"
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

bool ensureMqtt()
{
    if (client.connected())
        return true;
    while (!client.connected())
    {
        Serial.printf("The client %s connects to the ExtralityLab MQTT broker\n", client_id.c_str());
        bool isConnected = client.connect(client_id.c_str(), mqtt_username, mqtt_password);
        if (isConnected)
        {
            Serial.println("ExtralityLab MQTT broker connected");
            return true;
        }
        else
        {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

// Notify that there is a function called `mqttCallback`. Implementation is further below.
void mqttCallback(char *subscriber, byte *payload, unsigned int length);

// --------------------------------------------------
// Built-in LED functions
// --------------------------------------------------

void setBoardLed(uint8_t brightness)
{
#ifdef LED_TYPE_GPIO // ESP32 regular and S2 boards
    ledcWrite(PIN_LED_BOARD, brightness);
#endif

#ifdef LED_TYPE_RGB                             // ESP32 S3 Boards
    rgbLedWrite(PIN_LED_BOARD, brightness, brightness, brightness);
#endif
}

void setBoardLed(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef LED_TYPE_GPIO // ESP32 regular and S2 boards
    ledcWrite(PIN_LED_BOARD, red);
#endif

#ifdef LED_TYPE_RGB                             // ESP32 S3 Boards
    rgbLedWrite(PIN_LED_BOARD, red, green, blue);
#endif
}

////////////////////////////////////
//////////// SETUP
////////////////////////////////////

void setup()
{
    // Set software serial baud to 115200;
    Serial.begin(115200);
    while (!Serial && millis() < 5000)
        ;
    delay(100);

    //// Set up pinModes
    // Inputs
    pinMode(PIN_BTN, INPUT_PULLUP); // Built-in button // using INPUT_PULLUP: HIGH = not pressed (pullup), LOW = pressed

    // Outputs/Actuators
    ledcAttachChannel(PIN_LED_BOARD, LEDC_FRQ, LEDC_BIT, LEDC_CHN); // Built-in LED as Analog


    // Printing Board information
    Serial.println();
    Serial.println("=================================");
    Serial.print("Compiled for board: ");
    Serial.println(BOARD_NAME);
    Serial.println("=================================");
    
    ///////////////////
    // Connecting to a WiFi network
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
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

    subs = deviceMacID + subs_out_BoardLED;
    subs.toCharArray(subs_out_BoardLED, sizeof(subs_out_BoardLED));
    client.subscribe(subs_out_BoardLED);
    Serial.println(subs_out_BoardLED);
}

////////////////////////////////////
//////////// MAIN LOOP
////////////////////////////////////

void loop()
{
    // Ensure connections
    ensureMqtt();
    client.loop();

    // ---- topicBoardButton: Publish on change ---
    boardBtnState = digitalRead(PIN_BTN);
    if (boardBtnState != lastBoardBtnState)
    {
        lastBoardBtnState = boardBtnState;

        client.publish(topicBoardButton, String(boardBtnState).c_str());
        Serial.print("Board button: ");
        Serial.println(boardBtnState);

        ///////////
        //// EDIT: Here we manually also announce the topics of the actuator
        // that we would like to control with the input
        client.publish(subs_out_BoardLED, "toggle");
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
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        message += (char)payload[i];
    }
    Serial.println();
    Serial.println("-----------------------");

    ///// Select actuator based on subscriber's name:

    // ---- subs_out_BoardLED: Built-in Board LED as Digital or Analog---
    if (strcmp(subscriber, subs_out_BoardLED) == 0)
    {      
      Serial.println("Built-in LED...");
      if (message == "true")
      {
        lastLedState = true;
        setBoardLed(32);
      }
      else if (message == "false")
      {
        lastLedState = false;
        setBoardLed(0);
      }
      else if (message == "toggle")
      {
          if(lastLedState)
          {
              lastLedState = false;
              setBoardLed(0);
          }
          else
          {
            lastLedState = true;
              setBoardLed(32);
          }
      }
      else if (message == "red")
      {
        setBoardLed(32, 0, 0);
      }
      else if (message == "green")
      {
          setBoardLed(0, 32, 0);
      }
      else if (message == "blue")
      {
          setBoardLed(0, 0, 32);
      }
      else if (strchr(message.c_str(), ',')) // If message contains a comma
      { 
          // Try to read three integers separated by commas (spaces allowed)
          int r, g, b;
          if (sscanf(message.c_str(), " %d , %d , %d ", &r, &g, &b) == 3)
          {
              setBoardLed(r, g, b); // Set RGB color
          }
      }
      else
      {
          Serial.println("Error! Unknown message. Valid messages: ['true','false','toggle','red','green','blue','[r,g,b]']");
      }
    }
    else
    {
        Serial.println("Error! Unknown subscriber.");
    }
}