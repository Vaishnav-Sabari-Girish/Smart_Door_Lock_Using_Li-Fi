#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

struct UserData {
  uint16_t userId;
  char patternHash[65];      // SHA-256 hash (64 chars + null)
  uint32_t timestamp;
  uint8_t accessLevel;       // 0=guest, 1=user, 2=admin
  bool isActive;
};

class UserManager {
private:
  Preferences preferences;
  int userCount;
  
public:
  UserManager();
  bool initializeDatabase();
  int addUser(const char* patternHash, uint8_t accessLevel);
  bool removeUser(uint16_t userId);
  bool isValidUser(uint16_t userId, const char* patternHash);
  bool updateUserAccess(uint16_t userId, uint8_t newAccessLevel);
  int getUserCount();
  void clearAllUsers();
  UserData getUser(uint16_t userId);
  void logAccess(uint16_t userId, bool success);
};

extern UserManager userManager;

#endif
