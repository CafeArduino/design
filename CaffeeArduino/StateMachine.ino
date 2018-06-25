#include "libraries/arduino-fsm-modified/Fsm.h"
#include "libraries/PinChangeInterrupt/src/PinChangeInterrupt.h"
#include "SPI.h"

//internal variables
tokenId_t curToken;
coffee_t curCoffee;

//Times
#define CANCEL_AUTHENTICATED_TIME 5000
#define COFFEE_READY_WAIT_TIME 20000
#define AUTHENTICATED_TIME 5000
#define FAIL_TIME 10000
#define UU_TIME 10000

//Events - do not use 0!
#define AI_EVENT 2    //Authenticated input
#define COB_EVENT 3   //coffe brew
#define CAB_EVENT 4   // cancel button
#define UU_EVENT 5 // unknown user
#define FAIL_EVENT 10  // general failure catch all


void booting_entry();
void brewing_entry();

State state_ready(String("ready"), &ready_entry, NULL, NULL);
State state_authenticated(String("authenticated"), &authenticated_entry, NULL, &authenticated_exit);
//State state_brewing(String("brewing"), &brewing_entry, NULL, NULL); //go directly into coffee_ready state
State state_coffee_ready(String("coffee_ready"), &coffee_ready_entry, NULL, NULL);
State state_unknown_user(String("unknown_user"), &unknown_user_entry, NULL, NULL);
State state_fail(String("fail"), &fail_entry, NULL, NULL);

Fsm fsm_cm(&state_ready);

//-----------------------------------------------------------------------------------------------


void setup_statemachine() {
  logging("setup_statemachine");

  init_cm();
  init_interrupts();
  run_cm();
}




void loop() {
  //logging("loop");
  
  run_cm();

 
  if (tokenId_t token = checkForCard() != NO_CARD) {
    curToken = token;
    fsm_cm.trigger(AI_EVENT);
  }

  delay(250);

  logging(String("curToken: ") + curToken);
  logging(String("curButton") + curCoffee);
}


//---------------------------------------------------------------------------------------------------------------
void init_cm() {

  logging("init_cm");

  //von ready
  fsm_cm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  //von authenticated
  fsm_cm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
  fsm_cm.add_transition(&state_authenticated, &state_coffee_ready, COB_EVENT, NULL);
  fsm_cm.add_timed_transition(&state_authenticated, &state_ready, AUTHENTICATED_TIME, NULL);
  //von coffee_Ready
  fsm_cm.add_timed_transition(&state_coffee_ready, &state_ready, COFFEE_READY_WAIT_TIME, &on_coffee_got);
  fsm_cm.add_transition(&state_coffee_ready, &state_ready, CAB_EVENT, &on_no_coffee_got);
  //von unknown_user
  fsm_cm.add_timed_transition(&state_unknown_user, &state_ready, UU_TIME, NULL);
  fsm_cm.add_transition(&state_unknown_user, &state_ready, CAB_EVENT, NULL);
  //von fail
  fsm_cm.add_timed_transition(&state_fail, &state_ready, FAIL_TIME, NULL);
  fsm_cm.add_transition(&state_fail, &state_ready, CAB_EVENT, NULL);
}

void init_interrupts() {
  logging("init_interrupts");

  //Pins
  pinMode(BUTTON_CANCEL, OUTPUT);
  pinMode(BUTTON_NORMAL, OUTPUT);
  pinMode(BUTTON_NORMAL_DOUBLE, OUTPUT);
  pinMode(BUTTON_ESPRESSO, OUTPUT);
  pinMode(BUTTON_ESPRESSO_DOUBLE, OUTPUT);

  //interrupts
  attachPCINT(digitalPinToPCINT(BUTTON_CANCEL), cancelButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_NORMAL), normalButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_NORMAL_DOUBLE), normalDoubleButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_ESPRESSO), espressoButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_ESPRESSO_DOUBLE), espressoDoubleButtonPressed, FALLING);

}

void run_cm() {
  //logging("run_cm");
  fsm_cm.run_machine();
}

//-----------------------------------------------------------------------------------------------
//state functions

void ready_entry() {
  logging("ready_entry");
  gui.println("Bitte authentifizieren Sie sich");
}

void authenticated_entry() {
  logging("authenticated_entry");
  switch (authenticateToken(curToken)) {
    case OK: gui.println("Hallo!"); break;
    case USER_UNKNOWN: fsm_cm.trigger(UU_EVENT); break;
    case FAIL: fsm_cm.trigger(FAIL_EVENT); break;
  }
}

void authenticated_exit() {
  logging("authenticated_exit");
  gui.clear(); //not necessary
}

void fail_entry() {
  logging("fail_entry");
  gui.println("Es ist ein Feheler aufgetreten");
}

void unknown_user_entry() {
  logging("unknown_user_entry");
  gui.println("Unbekannter User. Bitte registrieren lassen.");
}

void coffee_ready_entry() {
  logging("coffee_ready_entry");
  gui.println("Wenn die Kaffeemaschine einen Fehler meldet, drücken Sie bitte 'Cancel'");
}

void on_no_coffee_got() {
  logging("on_no_coffee_got");
  gui.println("Es wird nichts abgerechnet. Überprüfen Sie die Kaffeemaschine auf Betriebsbereitschaft.");
}

void on_coffee_got() {
  logging("on_coffee_got");
  gui.print("Es wird Option "); gui.print(curCoffee); gui.println(" abgerechnet");
  switch(curCoffee) {
    case 4: case 6: incrementCoffeeCount(curToken, 1); break;
    case 5: case 7: incrementCoffeeCount(curToken, 2); break; 
  }
}

//---------------------------------------------------------------------------------------------------
//interrupt functions

void normalButtonPressed() {
  logging("normalButtonPressed");
  
  curCoffee = BUTTON_NORMAL;
  fsm_cm.trigger(COB_EVENT);
}

void normalDoubleButtonPressed() {
 logging("normalDoubleButtonPressed");
 
  curCoffee = BUTTON_NORMAL_DOUBLE;
  fsm_cm.trigger(COB_EVENT);
}

void espressoButtonPressed() {
  logging("espressoButtonPressed");
  
  curCoffee = BUTTON_ESPRESSO;
  fsm_cm.trigger(COB_EVENT);
}

void espressoDoubleButtonPressed() {
  logging("espressoDoubleButtonPressed"); 

  curCoffee = BUTTON_ESPRESSO_DOUBLE;
  fsm_cm.trigger(COB_EVENT);
}

void cancelButtonPressed() {
  logging("cancelButtonPressed"); 
 
  fsm_cm.trigger(CAB_EVENT);
}
