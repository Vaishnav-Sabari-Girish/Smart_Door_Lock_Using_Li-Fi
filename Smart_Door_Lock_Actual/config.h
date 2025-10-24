#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define LDR_PIN 34
#define RELAY_PIN 4
#define ENROLLMENT_BUTTON_PIN 5  // New: Button for enrollment mode
#define STATUS_LED_PIN 2         // New: Status LED

// System constants
#define PATTERN_LENGTH 8         // Increased from 5 for better security
#define THRESHOLD 2000           // LDR threshold
#define UNLOCK_DURATION 5000     // Door unlock duration (ms)
#define ENROLLMENT_TIMEOUT 30000 // 30 seconds enrollment window
#define TOKEN_VALIDITY 60000     // 60 seconds token validity
#define MAX_USERS 50            // Maximum users supported
#define BUTTON_PRESS_DURATION 5000 // 5 second button press for enrollment

// OOK Communication Parameters (as per paper)
#define OOK_SYMBOL_RATE 100     // 100 Hz symbol rate
#define DUTY_CYCLE_MS 5         // 50% duty cycle at 100Hz
#define SNR_THRESHOLD 12        // 12 dB SNR threshold
#define PATTERN_TIMEOUT 1000    // Timeout for each pattern step

// Security settings
#define MAX_FAILED_ATTEMPTS 3
#define LOCKOUT_DURATION 300000  // 5 minutes lockout

#endif
