// ESP32 Li-Fi Smart Door Lock with LDR and Relay
#include <Arduino.h>

// Pin definitions
#define LDR_PIN 34    // Analog pin for LDR
#define RELAY_PIN 4   // GPIO pin for relay
#define UNLOCK_DURATION 5000  // Duration to keep door unlocked (ms)

// Light pattern settings
#define PATTERN_LENGTH 5  // Number of light states in the pattern
#define THRESHOLD 2000    // Analog threshold for light detection (adjust based on LDR)
#define TIMEOUT 1000      // Timeout for each pattern step (ms)

// Define the expected light pattern (1 = light ON, 0 = light OFF)
int expectedPattern[PATTERN_LENGTH] = {1, 0, 1, 0, 1};  // Example: ON-OFF-ON-OFF-ON

// Variables
int detectedPattern[PATTERN_LENGTH];
unsigned long lastTime = 0;
bool relayActive = false;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Relay OFF (active LOW)
  pinMode(LDR_PIN, INPUT);

  Serial.println("ESP32 Li-Fi Smart Door Lock Initialized");
}

bool readLightState() {
  int ldrValue = analogRead(LDR_PIN);  // Read LDR analog value (0-4095)
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  return ldrValue > THRESHOLD;  // Return true if light is ON, false if OFF
}

bool checkPattern() {
  // Read the light pattern
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    unsigned long startTime = millis();
    bool currentState = readLightState();
    detectedPattern[i] = currentState ? 1 : 0;

    Serial.print("Detected State ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(detectedPattern[i]);

    // Wait for the next pattern step or timeout
    while (millis() - startTime < TIMEOUT) {
      if (readLightState() != currentState) {
        break;  // State changed, move to next step
      }
      delay(10);
    }
    delay(100);  // Small delay between steps
  }

  // Compare detected pattern with expected pattern
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    if (detectedPattern[i] != expectedPattern[i]) {
      return false;  // Pattern mismatch
    }
  }
  return true;  // Pattern matches
}

void loop() {
  // Check if relay is active and time to lock the door
  if (relayActive && millis() - lastTime > UNLOCK_DURATION) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn off relay (lock door)
    relayActive = false;
    Serial.println("Door Locked");
  }

  // Check for light pattern
  if (!relayActive && checkPattern()) {
    Serial.println("Valid Light Pattern Detected - Unlocking Door");
    digitalWrite(RELAY_PIN, LOW);  // Turn on relay (unlock door)
    relayActive = true;
    lastTime = millis();
  } else if (!relayActive) {
    Serial.println("Invalid or No Pattern Detected");
  }

  delay(500);  // Delay between checks
}
