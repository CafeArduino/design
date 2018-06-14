#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

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

tokenId_t checkForCard() {


  uint16_t code = 0;

  if ( ! mfrc522.PICC_IsNewCardPresent())
    return NO_CARD;

  if ( ! mfrc522.PICC_ReadCardSerial())
    return NO_CARD;

  for (byte i = 0; i < 5; i++)
    code = ((code + mfrc522.uid.uidByte[i]) * 2);

  if (code == 5988) {
    user1++;
    Serial.print("user1: authenticated");
    Serial.print(" | ");
    Serial.print(user1);
    Serial.print(" | ");
    Serial.print("card number: ");
    Serial.println(code);
    //delay(2000); //testzwecke
  }

  else if (code == 7364) {
    user2++;
    Serial.print("user2: authenticated");
    Serial.print(" | ");
    Serial.print(user2);
    Serial.print(" | ");
    Serial.print("card number: ");
    Serial.println(code);
    //delay(2000); //testzwecke
  }
  else {
    Serial.print("card number not known");
    Serial.print(" | ");
    Serial.print("card number: ");
    Serial.println(code);
    //delay(2000); //testzwecke
  }

  return (tokenId_t) code;
}


