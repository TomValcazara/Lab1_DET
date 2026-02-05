// Pin definitions
//(WARNING: Check your board pinout - Different on ESP32-S3)
const int PIN_POT = A0;   // Potentiometer input
const int PIN_SERVO = 10; // GPIO connected to servo signal (pick a PWM-capable pin)

const int SERVO_FRQ=50;  //define the pwm frequency 50Hz
const int SERVO_BIT=12;  //define the pwm precision 12 bits
const int SERVO_CHN=0;   //define the pwm channel 0

int potValue = 0; // variable to store the potentiometer position
int angle = 0;    // variable to store the servo position

// Notify that there will be a function further below called `servo_set_angle()`
void servo_set_angle(int angle);

// Setup
void setup() {
  pinMode(PIN_POT, INPUT);

  // Force to read analog inputs between 0-4095. Default may be 13 bits resolution
  analogReadResolution(12);

  // Attach the servo
  ledcAttachChannel(PIN_SERVO, SERVO_FRQ, SERVO_BIT, SERVO_CHN);

  Serial.println("Basic 3: ESP32-S2 PWM with servo controlled with potentiometer");

  Serial.begin(115200);
}

// Loop
void loop() {
  // ESP32 analog range: 0–4095
  potValue = analogRead(PIN_POT);
  // Map potentiometer to servo angle
  angle = map(potValue, 0, 4095, 0, 180);

  // Move servo
  servo_set_angle(angle);

  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print("  Servo angle: ");
  Serial.println(angle);

  // Small delay for smooth movement
  delay(15); 
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
