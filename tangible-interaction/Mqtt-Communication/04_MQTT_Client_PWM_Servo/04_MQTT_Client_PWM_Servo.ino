/*
  Project: 04_MQTT_Client_PWM_Servo
  Author: Luis Quintero
  Date: 2025-10-03
  Board: ESP32-S2 (Adapt Pin Layout for ESP32 or ESP32-S3)
  Description:
    Basic sketch demonstrating ANALOG inputs and outputs 
    with MQTT. 
    
    Behavior:
    - The potentiometer controls the servo's angle
    
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
const int PIN_POT = A0;    // Potentiometer input pin
int potValue = 0;    // variable to store the potentiometer value
int lastPotValue = -1;    // for comparison to detect change
const int changeThreshold = 20;     // minimum change to ignore tiny jitter
const unsigned long minIntervalMs = 200; // to control publish rate
unsigned long lastPotSentMs = 0;  // counter for publish rate

// PIN OUTPUT SERVO
const int PIN_SERVO = 10; // GPIO connected to servo signal (pick a PWM-capable pin)
const int SERVO_FRQ = 50;  //define the pwm frequency 50Hz
const int SERVO_BIT = 12;  //define the pwm precision 12 bits
const int SERVO_CHN = 1;   //define the pwm different than LEDC_CHN
int servoAngle = 0;    // variable to store the servo position


////////////////////////////////////
//////////// MQTT SETUP
////////////////////////////////////

//// Topics (Inputs sent to MQTT Broker)
// Digital
// char topicBoardButton[50] = "/D0_BoardBtn";

// Analog
char topicA0_in_pot[50] = "/A0_in_pot";

//// Subscribers (Actuators received from MQTT Broker)
//Digital
// N/A

//PWM
char subsD10_pwm_servo[50] = "/PWM10_servo";

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


// Custom function to set the angle of the servo
void servo_set_angle(int angle) {
  if (angle > 180 || angle < 0)
    return;

  // Thresholds needed to move the servo to required position
  // With 12 bits, a cycle is 20ms = 4095
  int minPwm = 103; // duty cycle of 0.5ms corresponds to 0 deg = (0.5ms*4095)/20ms
  int maxPwm = 512; // duty cycle of 2.5ms corresopnds to 180 deg = (2.5ms*4095)/20ms
  long pwm_value = map(angle, 0, 180, minPwm, maxPwm);

  ledcWrite(PIN_SERVO, pwm_value);
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
  pinMode(PIN_POT, INPUT); // Analog Potentiometer
  analogReadResolution(12); // Force read in 12 bits

  // Outputs/Actuators
  
  // Setup servo as PWM
  ledcAttachChannel(PIN_SERVO, SERVO_FRQ, SERVO_BIT, SERVO_CHN);

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

  topic = deviceMacID + topicA0_in_pot;
  topic.toCharArray(topicA0_in_pot, sizeof(topicA0_in_pot));
  Serial.println(topicA0_in_pot);

  ///////////////////
  ///// Subscribe to topics (one per actuator)
  Serial.println("\t[[MQTT subscriptions]]");

  // Build the subs strings, preceding the client_id
  String subs;

  subs = deviceMacID + subsD10_pwm_servo;
  subs.toCharArray(subsD10_pwm_servo, sizeof(subsD10_pwm_servo));
  client.subscribe(subsD10_pwm_servo);
  Serial.println(subsD10_pwm_servo);
}

////////////////////////////////////
//////////// MAIN LOOP
////////////////////////////////////

void loop() {

  // Ensure connections
  ensureMqtt();
  client.loop();

  // ---- topicA0_in_pot: Publish on change + hysteresis + minInterval ---
  potValue = analogRead(PIN_POT); 
  unsigned long now = millis();

  if ((abs(potValue - lastPotValue) > changeThreshold && (now - lastPotSentMs) > minIntervalMs) || lastPotValue < 0) {
    lastPotValue = potValue;
    lastPotSentMs = now;

    char rawValueText[8]; // Parse to string
    itoa(potValue, rawValueText, 10);
    client.publish(topicA0_in_pot, rawValueText);

    ///////////
    //// EDIT: Here we manually also announce the topics of the actuator
    // that we would like to control with the input
    int potValueServoAngle = map(potValue, 0, 4095, 0, 180); // Map potentiometer to servo angle
    char servoValueText[8]; // Parse to string
    itoa(potValueServoAngle, servoValueText, 10);
    client.publish(subsD10_pwm_servo, servoValueText);
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
  if (strcmp(subscriber, subsD10_pwm_servo) == 0)
  {
    Serial.println("Servo Motor [0-180] - Value: ");
    int value = message.toInt();
    Serial.println(value);

    if (value >= 0 && value <= 180) {
      servo_set_angle(value);
    } else{
      Serial.println("Error! Invalid message value. Valid range: [0-180]");
    }
  } else {
    Serial.println("Error! Unknown subscriber.");
  }
}