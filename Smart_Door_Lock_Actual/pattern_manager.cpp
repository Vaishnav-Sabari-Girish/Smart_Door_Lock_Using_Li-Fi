#include "pattern_manager.h"
#include "config.h"

PatternManager patternManager;

PatternManager::PatternManager() {
  currentMode = MODE_NORMAL;
  enrollmentStartTime = 0;
  buttonPressStart = 0;
  buttonPressed = false;
}

bool PatternManager::readLightState() {
  int ldrValue = analogRead(LDR_PIN);
  return ldrValue > THRESHOLD;
}

String PatternManager::capturePattern() {
  Serial.println("Capturing light pattern...");
  
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    unsigned long startTime = millis();
    bool currentState = readLightState();
    detectedPattern[i] = currentState ? 1 : 0;
    
    Serial.printf("Pattern step %d: %d\n", i, detectedPattern[i]);
    
    // Wait for pattern timing (100Hz = 10ms per symbol with 50% duty cycle)
    delay(DUTY_CYCLE_MS);
    
    // Check for state change or timeout
    while (millis() - startTime < PATTERN_TIMEOUT) {
      if (readLightState() != currentState) {
        break;
      }
      delay(1);
    }
  }
  
  return patternToString(detectedPattern);
}

void PatternManager::checkEnrollmentButton() {
  bool currentButtonState = !digitalRead(ENROLLMENT_BUTTON_PIN); // Assuming pull-up
  
  if (currentButtonState && !buttonPressed) {
    buttonPressed = true;
    buttonPressStart = millis();
  } else if (!currentButtonState && buttonPressed) {
    buttonPressed = false;
    if (millis() - buttonPressStart >= BUTTON_PRESS_DURATION) {
      enterEnrollmentMode();
    }
  }
}

void PatternManager::enterEnrollmentMode() {
  currentMode = MODE_ENROLLMENT;
  enrollmentStartTime = millis();
  Serial.println("Entering enrollment mode - 30 seconds to enroll new user");
}

void PatternManager::exitEnrollmentMode() {
  currentMode = MODE_NORMAL;
  Serial.println("Exiting enrollment mode");
}

SystemMode PatternManager::getCurrentMode() {
  // Check enrollment timeout
  if (currentMode == MODE_ENROLLMENT && 
      millis() - enrollmentStartTime > ENROLLMENT_TIMEOUT) {
    exitEnrollmentMode();
  }
  
  return currentMode;
}

void PatternManager::updateStatusLED() {
  switch (currentMode) {
    case MODE_NORMAL:
      digitalWrite(STATUS_LED_PIN, LOW);  // LED off
      break;
    case MODE_ENROLLMENT:
      // Blink LED for enrollment mode
      digitalWrite(STATUS_LED_PIN, (millis() / 500) % 2);
      break;
    case MODE_LOCKOUT:
      digitalWrite(STATUS_LED_PIN, HIGH); // LED solid on
      break;
  }
}

String PatternManager::patternToString(int* pattern) {
  String result = "";
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    result += String(pattern[i]);
  }
  return result;
}
