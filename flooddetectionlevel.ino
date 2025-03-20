#define BLYNK_TEMPLATE_ID "TMPL2g10CJQVF"
#define BLYNK_TEMPLATE_NAME "Hydrosentinel 1"
#define BLYNK_AUTH_TOKEN "FzjDYKvMkYV5HsqCvgUp_pvSWtHim3Oz"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Replace with your network credentials
char ssid[] = "**********";

char pass[] = "*******";


// Ultrasonic sensor pins
const int trigPin = 13;
const int echoPin = 12;

// Variables
long duration;
float distance; // in cm
float floodLevel; // in cm (distance from sensor to water)
float maxDistance = 50.0; // e.g., maximum distance in cm
bool alertSent = false; // To prevent multiple alerts for the same condition

// Threshold for flood alert
const float floodThreshold = 30.0; // Alert if flood level is less than 20 cm

// Timer
BlynkTimer timer;

// Function to read ultrasonic sensor
void readSensor() {
  // Trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo time
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance (speed of sound: 34300 cm/s)
  distance = (duration * 0.0343) / 2;

  // Calculate flood level
  floodLevel = maxDistance - distance;

  // Send data to Blynk
  Blynk.virtualWrite(V0, floodLevel); // V0 is a virtual pin

  // Check flood level and send notification
  if (floodLevel < floodThreshold && !alertSent) {
    Blynk.logEvent("flood_alert", "Flood level critically high!"); // Use the Blynk event system
    Serial.println("Flood alert notification sent!");
    alertSent = true; // Prevent duplicate alerts
  } else if (floodLevel >= floodThreshold) {
    alertSent = false; // Reset alert status if level is back to normal
  }
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Ultrasonic sensor setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Blynk setup
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup timer to read sensor data every 1 second
  timer.setInterval(1000L, readSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
