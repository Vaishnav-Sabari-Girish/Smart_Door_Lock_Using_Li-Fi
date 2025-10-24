#include "security.h"
#include "config.h"

SecurityManager securityManager;

SecurityManager::SecurityManager() {
  failedAttempts = 0;
  lastFailedTime = 0;
  lastTokenTime = 0;
  isLockedOut = false;
}

String SecurityManager::generateHash(const String& pattern) {
  mbedtls_sha256_context sha256_ctx;
  unsigned char hash[32];
  char hashString[65];
  
  mbedtls_sha256_init(&sha256_ctx);
  mbedtls_sha256_starts(&sha256_ctx, 0);
  mbedtls_sha256_update(&sha256_ctx, (unsigned char*)pattern.c_str(), pattern.length());
  mbedtls_sha256_finish(&sha256_ctx, hash);
  mbedtls_sha256_free(&sha256_ctx);
  
  // Convert to hex string
  for (int i = 0; i < 32; i++) {
    sprintf(&hashString[i*2], "%02x", hash[i]);
  }
  hashString[64] = '\0';
  
  return String(hashString);
}

bool SecurityManager::verifyPattern(const String& pattern, const String& expectedHash) {
  String patternHash = generateHash(pattern);
  return (patternHash == expectedHash);
}

bool SecurityManager::isTokenValid(unsigned long tokenTime) {
  return (millis() - tokenTime) < TOKEN_VALIDITY;
}

bool SecurityManager::checkRateLimit() {
  if (isLockedOut && (millis() - lastFailedTime) > LOCKOUT_DURATION) {
    isLockedOut = false;
    failedAttempts = 0;
    Serial.println("Security lockout expired");
  }
  
  return !isLockedOut;
}

void SecurityManager::recordFailedAttempt() {
  failedAttempts++;
  lastFailedTime = millis();
  
  if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
    isLockedOut = true;
    Serial.printf("Security lockout activated after %d failed attempts\n", failedAttempts);
  }
}

void SecurityManager::resetFailedAttempts() {
  failedAttempts = 0;
  isLockedOut = false;
}
