// #include <Arduino.h>

// // ---- pin mapping ----
// #define TRIG_PIN 5
// #define ECHO_PIN 18

// // ---- sensor settings ----
// #define SOUND_SPEED 0.0343  // Speed of sound in cm per microsecond

// void setup() {
//   Serial.begin(115200);

//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);

//   digitalWrite(TRIG_PIN, LOW);

//   Serial.println("HC-SR04 ultrasonic sensor test started");
// }

// void loop() {
//   // ---- send trigger pulse ----
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);

//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   // ---- read echo duration ----
//   long duration = pulseIn(ECHO_PIN, HIGH, 30000);

//   // ---- check result and calculate distance ----
//   if (duration == 0) {
//     Serial.println("No echo detected");
//   } else {
//     float distanceCm = duration * SOUND_SPEED / 2.0;

//     Serial.print("Distance: ");
//     Serial.print(distanceCm);
//     Serial.println(" cm");
//   }

//   delay(500);
// }