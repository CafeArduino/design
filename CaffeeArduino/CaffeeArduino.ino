#include "libraries/arduino-fsm-modified/Fsm.h"
#include "SPI.h"

typedef uint32_t tokenId_t;
typedef uint8_t coffee_t;

// Returns:
//   - 0x0 (NO_CARD) if no card present
//   - 4 first bytes of the ID of the card
// Author: Lars
tokenId_t checkForCard();

///////////////////////////////////////////////////////
// Author: José | Daniel
// EEPROM Structure:
// For each user: 4 byte ID + 2 byte Coffee counter
// Functions:

enum Results{ OK                = 0,
              FAIL              = -1,
              NOT_FOUND         = -2,
              NO_EMPTY_SLOT     = -3,
              COUNTER_OVERFLOW  = -4,
              USER_UNKNOWN      = -5,
              NO_CARD           = 0,
              EEPROM_FULL       = 1,
              USER_EXISTS       = 2,
              };

// Adds a new user token
// Returns:
//   - OK
//   - EEPROM_FULL
//   - USER_EXISTS
//   - EEPROM_FAIL
int addToken(tokenId_t tokenId);

// Deletes a user token
// Returns:
//   - OK
//   - EEPROM_FAIL
int deleteToken(tokenId_t tokenId);

// Returns:
//   - New Value
//   - EEPROM_FAIL
//   - COUNTER_OVERFLOW
// Count valid values (1 or 2)
int incrementCoffeeCount(tokenId_t tokenId, int count);

int getCoffeeCounter(tokenId_t tokenId);
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
// Author: Florian
// Buttons
#if defined(__AVR_ATmega2560__)  || defined(__AVR_ATmega1280__)
  #warning "*** Compiling for Arduino MEGA ***"
  #define BUTTON_CANCEL A8
  #define BUTTON_NORMAL A9
  #define BUTTON_NORMAL_DOUBLE A10
  #define BUTTON_ESPRESSO A11
  #define BUTTON_ESPRESSO_DOUBLE A12
#else 
  #warning "*** Compiling for Arduino UNO ***"
  #define BUTTON_CANCEL 3
  #define BUTTON_NORMAL 4
  #define BUTTON_NORMAL_DOUBLE 5
  #define BUTTON_ESPRESSO 6
  #define BUTTON_ESPRESSO_DOUBLE 7
#endif


// Returns:
//   - OK
//   - FAIL
int brewCoffee(coffee_t type);
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
// Author: Lars

// Returns:
//   - OK
//   - USER_UNKNOWN
//   - FAIL
int authenticateToken(tokenId_t tokenId);
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
// Author: 
void displayError(int errorCode);
void displayString(String s);

// All functions (except this) have to all this function and the end
// s is composed of "function_name + function_argument(s) + return_value"
void logging(String s);
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// Author: Daniel
class Screen {
  public:
    void print(String s){};
    void print(int32_t){};
    void println(String s){};
    void println(int32_t){};
    void clear(){};
    /* ... */
  private:
} gui;

 #if defined(__AVR_ATmega2560__)  || defined(__AVR_ATmega1280__)
  #warning "*** Compiling for Arduino MEGA ***"
#else 
  #warning "*** Compiling for Arduino UNO ***"
  auto Serial1 = Serial;
#endif

void setup() {
  // setup serial ports
  Serial.begin(9600);
  Serial1.begin(9600);     // start serial communication at 9600bps
  
  logging("setup"); 

  //card reader
  setup_authenticate();

  // state machine  
  setup_statemachine();

  // coffee machine
  setup_CoffeeMachine();

  // add a token; have no token management yet
  addToken(2719172356);
  logging(String("users: ") + getNumberUsers());
}

/*
void loop() {

  run_cm();
  if (tokenId_t token = checkForCard() != 0) {
    curToken = token;
    fsm_cm.trigger(AI_EVENT);
  }

  delay(50);
}
*/
