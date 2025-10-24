#ifndef SECURITY_H
#define SECURITY_H

#include <Arduino.h>
#include <mbedtls/sha256.h>

class SecurityManager {
private:
  int failedAttempts;
  unsigned long lastFailedTime;
  unsigned long lastTokenTime;
  bool isLockedOut;
  
public:
  SecurityManager();
  String generateHash(const String& pattern);
  bool verifyPattern(const String& pattern, const String& expectedHash);
  bool isTokenValid(unsigned long tokenTime);
  bool checkRateLimit();
  void recordFailedAttempt();
  void resetFailedAttempts();
  String generateChallenge();
  bool verifyResponse(const String& challenge, const String& response, const String& userHash);
  void updateSecurityStatus();
};

extern SecurityManager securityManager;

#endif
