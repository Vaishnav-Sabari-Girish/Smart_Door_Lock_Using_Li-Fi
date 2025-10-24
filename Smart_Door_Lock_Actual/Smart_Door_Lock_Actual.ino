#include "config.h"
#include "pattern_manager.h"
#include "security.h"
#include "user_manager.h"

// Global variables
unsigned long lastTime = 0;
bool relayActive = false;
unsigned long lastPatternCheck = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ENROLLMENT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  
  // Initialize relay (locked state)
  digitalWrite(RELAY_PIN, HIGH);
  
  // Initialize managers
  userManager.initializeDatabase();
  
  Serial.println("ESP32 Li-Fi Enhanced Smart Door Lock Initialized");
  Serial.printf("System supports up to %d users\n", MAX_USERS);
  Serial.printf("Current enrolled users: %d\n", userManager.getUserCount());
  Serial.println("Hold enrollment button for 5 seconds to enter enrollment mode");
}

void handleEnrollmentMode() {
  static bool waitingForPattern = true;
  static int enrollmentAttempts = 0;
  static String capturedPatterns[3]; // Require 3 consecutive matches
  
  if (waitingForPattern) {
    Serial.println("Enrollment mode active - transmit your pattern 3 times");
    waitingForPattern = false;
  }
  
  // Check for light pattern
  String pattern = patternManager.capturePattern();
  if (pattern.length() > 0) {
    capturedPatterns[enrollmentAttempts] = pattern;
    enrollmentAttempts++;
    
    Serial.printf("Enrollment attempt %d/3 captured\n", enrollmentAttempts);
    
    if (enrollmentAttempts >= 3) {
      // Verify all 3 patterns match
      if (capturedPatterns[0] == capturedPatterns[1] && 
          capturedPatterns[1] == capturedPatterns[2]) {
        
        // Generate hash and store user
        String patternHash = securityManager.generateHash(pattern);
        int userId = userManager.addUser(patternHash.c_str(), 1); // Default user level
        
        if (userId > 0) {
          Serial.printf("User enrolled successfully with ID: %d\n", userId);
          // Flash LED to indicate success
          for (int i = 0; i < 6; i++) {
            digitalWrite(STATUS_LED_PIN, i % 2);
            delay(200);
          }
        } else {
          Serial.println("Enrollment failed - database full");
        }
      } else {
        Serial.println("Pattern mismatch - enrollment failed");
      }
      
      // Reset enrollment
      enrollmentAttempts = 0;
      waitingForPattern = true;
      patternManager.exitEnrollmentMode();
    }
    
    delay(1000); // Delay between enrollment attempts
  }
}

void handleAuthenticationMode() {
  // Check for light pattern every 500ms to avoid excessive processing
  if (millis() - lastPatternCheck > 500) {
    String pattern = patternManager.capturePattern();
    
    if (pattern.length() > 0) {
      String patternHash = securityManager.generateHash(pattern);
      
      // Check against all enrolled users
      bool accessGranted = false;
      for (uint16_t userId = 1; userId <= userManager.getUserCount(); userId++) {
        if (userManager.isValidUser(userId, patternHash.c_str())) {
          if (securityManager.checkRateLimit()) {
            Serial.printf("Access granted for User ID: %d\n", userId);
            digitalWrite(RELAY_PIN, LOW);  // Unlock door
            relayActive = true;
            lastTime = millis();
            accessGranted = true;
            securityManager.resetFailedAttempts();
            userManager.logAccess(userId, true);
            break;
          } else {
            Serial.println("System locked out due to too many failed attempts");
            userManager.logAccess(userId, false);
          }
        }
      }
      
      if (!accessGranted && securityManager.checkRateLimit()) {
        Serial.println("Invalid pattern detected");
        securityManager.recordFailedAttempt();
        userManager.logAccess(0, false); // Log failed attempt with userId 0
      }
    }
    
    lastPatternCheck = millis();
  }
}

void loop() {
  // Check enrollment button
  patternManager.checkEnrollmentButton();
  
  // Update status LED
  patternManager.updateStatusLED();
  
  // Handle door lock timing
  if (relayActive && millis() - lastTime > UNLOCK_DURATION) {
    digitalWrite(RELAY_PIN, HIGH);  // Lock door
    relayActive = false;
    Serial.println("Door locked automatically");
  }
  
  // Handle different modes
  switch (patternManager.getCurrentMode()) {
    case MODE_ENROLLMENT:
      handleEnrollmentMode();
      break;
      
    case MODE_NORMAL:
      if (!relayActive) {  // Only check for patterns when door is locked
        handleAuthenticationMode();
      }
      break;
      
    case MODE_LOCKOUT:
      // System locked out - just update security status
      securityManager.updateSecurityStatus();
      if (securityManager.checkRateLimit()) {
        patternManager.setMode(MODE_NORMAL);
      }
      break;
  }
  
  delay(10); // Small delay to prevent excessive CPU usage
}
