#include <SPI.h>
#include <MFRC522.h>

#if defined(__AVR_ATmega2560__)  || defined(__AVR_ATmega1280__)
#define SS_PIN 53
#else 
#define SS_PIN 10
#endif
#define RST_PIN 9

#define MASTER_TOKEN 1505981968

uint16_t user1 = 0;
uint16_t user2 = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);


void setup_authenticate() {
  logging("setup_authenticate");
  // Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}


int authenticateToken(tokenId_t tokenId) {
  if (NOT_FOUND == tokenPosition(tokenId)) 
    return USER_UNKNOWN;      
  else
    return OK;
}

void manageTokens() {
logging(__FUNCTION__);

  delay(500);
  return;
}


tokenId_t hash_uuid() {
  const byte len = 4;
  byte buf[len] = {0}; 
  uint32_t hash;
  
/* 
  for (byte i = 0; i < 5; i++)
    hash = ((hash + mfrc522.uid.uidByte[i]) * 2);
*/
  // take first 4 bytes
  for (byte i=0; i<len; i++) 
    buf[i] = mfrc522.uid.uidByte[i]; 
  memcpy(&hash, buf, 4);

  return (tokenId_t) hash; 
}

tokenId_t checkForCard() {
  tokenId_t code = 0;

  // logging("CheckForCard");
  
  // for debug always authenticate: return (tokenId_t) 1234;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return NO_CARD;
  }

  if ( ! mfrc522.PICC_ReadCardSerial())
    return NO_CARD;

  code = hash_uuid();

  if (code == MASTER_TOKEN) {
    manageTokens();
    return NO_CARD;
  }


  // found a token
  logging(String("checkForCard: token ") + code );
  //logging(String("checkForCard: index ") + tokenPosition(code) );
  //logging(String("CheckForCard "));
  //Serial.println(code, HEX);

  // now, check if token is known already
  if (tokenPosition(code) != NOT_FOUND) {
    logging("checkForCard: valid token ");
    return (tokenId_t) code; 
  }
  
  return NO_CARD;
}


