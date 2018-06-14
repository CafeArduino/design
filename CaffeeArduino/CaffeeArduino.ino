typedef uint32_t tokenId_t;
typedef uint8_t coffee_t;

// Returns:
//   - 0x0 if no card present
//   - 4 first bytes of the ID of the card
// Author: Lars
tokenId_t checkForCard();

///////////////////////////////////////////////////////
// Author: José | Daniel
// EEPROM Structure:
// For each user: 4 byte ID + 2 byte Coffee counter
// Functions:

enum ()...;

// Adds a new user token
// Returns:
//   - SUCCESS
//   - EEPROM_FULL
//   - USER_EXISTS
//   - EEPROM_FAIL
int addToken(tokenId_t tokenId);

// Deletes a user token
// Returns:
//   - SUCCESS
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
#define BUTTON_CANCEL 0
#define BUTTON_NORMAL 4
#define BUTTON_NORMAL_DOUBLE 5
#define BUTTON_ESPRESSO 6
#define BUTTON_ESPRESSO_DOUBLE 7

// Returns:
//   - SUCCESS
//   - FAIL
int brewCoffee(coffee_t type);
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
// Author: Lars

// Returns:
//   - SUCCESS
//   - USER_UNKNOWN
//   - FAIL
int authenticateToken(tokenId_t tokenId);
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
// Author: 
void displayError(int errorCode);
void displayString(string s);

// All functions (except this) have to all this function and the end
// s is composed of "function_name + function_argument(s) + return_value"
void logging(string s);
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// Author: Daniel
class Screen {
  public:
    void print(string s);
    void println(string s);
    void clear();
    /* ... */
  private:
} gui;


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
