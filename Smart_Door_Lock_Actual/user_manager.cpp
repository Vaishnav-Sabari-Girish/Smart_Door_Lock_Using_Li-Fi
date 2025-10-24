#include "user_manager.h"
#include "config.h"

UserManager userManager;

UserManager::UserManager() : userCount(0) {}

bool UserManager::initializeDatabase() {
  preferences.begin("door_users", false);
  userCount = preferences.getInt("user_count", 0);
  Serial.printf("User database initialized. Current users: %d\n", userCount);
  return true;
}

int UserManager::addUser(const char* patternHash, uint8_t accessLevel) {
  if (userCount >= MAX_USERS) {
    Serial.println("Maximum users reached");
    return -1;
  }
  
  uint16_t newUserId = userCount + 1;
  String userKey = "user_" + String(newUserId);
  
  UserData newUser;
  newUser.userId = newUserId;
  strcpy(newUser.patternHash, patternHash);
  newUser.timestamp = millis();
  newUser.accessLevel = accessLevel;
  newUser.isActive = true;
  
  // Store user data
  preferences.putBytes(userKey.c_str(), &newUser, sizeof(UserData));
  userCount++;
  preferences.putInt("user_count", userCount);
  
  Serial.printf("User %d enrolled with access level %d\n", newUserId, accessLevel);
  return newUserId;
}

bool UserManager::isValidUser(uint16_t userId, const char* patternHash) {
  String userKey = "user_" + String(userId);
  UserData userData;
  
  if (preferences.getBytes(userKey.c_str(), &userData, sizeof(UserData)) == 0) {
    return false;
  }
  
  return (userData.isActive && strcmp(userData.patternHash, patternHash) == 0);
}

void UserManager::logAccess(uint16_t userId, bool success) {
  String logKey = "log_" + String(millis());
  String logData = String(userId) + "," + String(success ? 1 : 0) + "," + String(millis());
  preferences.putString(logKey.c_str(), logData);
  Serial.printf("Access log: User %d, Success: %s, Time: %lu\n", 
                userId, success ? "true" : "false", millis());
}

int UserManager::getUserCount() {
  return userCount;
}
