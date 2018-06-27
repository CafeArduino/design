/**************************************************************/
/**************************************************************/
/*                      EEPROM functions                      */
/**************************************************************/
/* EEPROM Structure:                                          */
/* For each user: 4 byte ID + 2 byte Coffee counter           */
/**************************************************************/

#include <EEPROM.h>

#define MAX_USERS 32

void clearEEPROM() {
  for(int i=0; i<EEPROM.length(); i++) {
    EEPROM.write(i, 0x00);
  }
}

int getNumberUsers() {
  return EEPROM.read(0x00);
}

// Return the index in memory
int tokenPosition(tokenId_t id) {
  uint32_t val;
  uint8_t* p1 = (uint8_t*)&id;
  uint8_t* p2 = (uint8_t*)&val;

  logging(__FUNCTION__);
  for(int i=1; i<=MAX_USERS; i++) {
    p2[0] = EEPROM.read((8*i)+0);
    p2[1] = EEPROM.read((8*i)+1);
    p2[2] = EEPROM.read((8*i)+2);
    p2[3] = EEPROM.read((8*i)+3);

    //Serial.println(id, HEX); Serial.println(val, HEX);
    if(id == val) { // if equal, it was found
      return i;     // return the index
    }
  }

  return NOT_FOUND;
}

int findEmptySlot() {
  uint32_t val;
  uint8_t* p = (uint8_t*)&val;

  for(int i=1; i<=MAX_USERS; i++) {
    p[0] = EEPROM.read((8*i)+0);
    p[1] = EEPROM.read((8*i)+1);
    p[2] = EEPROM.read((8*i)+2);
    p[3] = EEPROM.read((8*i)+3);

    if(0x00000000 == val) { // if empty slot found
      return i;     // return the index
    }
  }

  return NO_EMPTY_SLOT;
}

// Deletes a user token
// Returns:
//   - OK
//   - FAIL
int deleteToken(tokenId_t id) {
  int index = tokenPosition(id);
  if(index > 0) {
    for(int i=0; i<8; i++) {
      EEPROM.write((index*8)+i, 0x00);
    }

    // Update number of users
    uint8_t n = EEPROM.read(0);
    n--;
    EEPROM.write(0, n);
    
    return OK;
  }
  else {
    return FAIL; 
  }
}

// Adds a new user token
// Returns:
//   - OK
//   - EEPROM_FULL
//   - USER_EXISTS
int addToken(tokenId_t id) {
  // Max number of users reached, it is not possible to add more
  if(getNumberUsers() >= MAX_USERS) {
    return EEPROM_FULL;
  }

  if(tokenPosition(id) != NOT_FOUND) {
    return USER_EXISTS;
  }

  int index = findEmptySlot();
  if(index == NO_EMPTY_SLOT) {
    // This should not be executed because we already checked
    // that maximum amount of users was not reached
    return EEPROM_FULL;
  }

  // write the tag id in the first 4 bytes
  uint8_t* p = (uint8_t*)&id;
  EEPROM.write((index*8)+0, p[0]);
  EEPROM.write((index*8)+1, p[1]);
  EEPROM.write((index*8)+2, p[2]);
  EEPROM.write((index*8)+3, p[3]);

  // empty the coffee count
  EEPROM.write((index*8)+4, 0x00);
  EEPROM.write((index*8)+5, 0x00);
  EEPROM.write((index*8)+6, 0x00);
  EEPROM.write((index*8)+7, 0x00);

  // Update number of users
  uint8_t n = EEPROM.read(0);
  n++;
  EEPROM.write(0, n);
    
  return OK;
}

// Return the number of coffes of the user
// or FAIL if not found
int getCoffeeCounter(tokenId_t tokenId) {
  uint16_t count;
  
  int index = tokenPosition(tokenId);
  if(index > 0) {

    uint8_t* p = (uint8_t*)&count;
    p[0] = EEPROM.read((index*8)+4);
    p[1] = EEPROM.read((index*8)+5);
    
    return count;
  }
  else {
    return FAIL;
  }
}

int incrementCoffeeCount(tokenId_t tokenId, int count) {
  int _count = getCoffeeCounter(tokenId);

  // check for error reading current cofee count
  if(count < 0) {
    return FAIL;
  }

  uint16_t _count16 = (uint16_t)_count;
  _count16 += (uint16_t)count;

  // check for counter overflow
  if(_count16 < (uint16_t)_count) {
    return COUNTER_OVERFLOW;
  }

  // Write the updated coffee counter for the given user
  uint8_t* p = (uint8_t*)&_count16;
  int index = tokenPosition(tokenId);
  EEPROM.write((index*8)+4, p[0]);
  EEPROM.write((index*8)+5, p[1]);

  return OK;
}

void eepromStatus() {
  uint32_t val;
  uint8_t* p = (uint8_t*)&val;
  uint16_t count;
  uint8_t* c = (uint8_t*)&count;

  int slots = 0;
  int n = EEPROM.read(0x00);
  Serial.print("Registered Users: ");
  Serial.println(n);
 

  for(int i=1; i<=MAX_USERS; i++) {
    p[0] = EEPROM.read((8*i)+0);
    p[1] = EEPROM.read((8*i)+1);
    p[2] = EEPROM.read((8*i)+2);
    p[3] = EEPROM.read((8*i)+3);
    c[0] = EEPROM.read((i*8)+4);
    c[1] = EEPROM.read((i*8)+5);

    Serial.print("Slot ");
    Serial.print(i);
    if(0x00000000 == val) { // if empty slot found
      Serial.println(": EMPTY");
    }
    else {
      Serial.print(" TOKEN: ");
      Serial.print(val);
      Serial.print(", coffees: ");
      Serial.println(count);
      slots++;
    }
  }

  if(n == slots) {
    Serial.println("GOOD: Register users and used slots is equal");
  }
  else {
    Serial.println("BAD: Register users and used slots is different");
  }
}

/**************************************************************/
