#ifndef PATTERN_MANAGER_H
#define PATTERN_MANAGER_H

#include <Arduino.h>

enum SystemMode {
  MODE_NORMAL,
  MODE_ENROLLMENT,
  MODE_LOCKOUT
};

class PatternManager {
private:
  int detectedPattern[PATTERN_LENGTH];
  SystemMode currentMode;
  unsigned long enrollmentStartTime;
  unsigned long buttonPressStart;
  bool buttonPressed;
  
public:
  PatternManager();
  bool readLightState();
  String capturePattern();
  bool verifyPattern(const String& expectedPattern);
  void checkEnrollmentButton();
  void enterEnrollmentMode();
  void exitEnrollmentMode();
  SystemMode getCurrentMode();
  void setMode(SystemMode mode);
  void updateStatusLED();
  String patternToString(int* pattern);
  void stringToPattern(const String& str, int* pattern);
};

extern PatternManager patternManager;

#endif
