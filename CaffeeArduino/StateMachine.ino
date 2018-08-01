#include "libraries/arduino-fsm-modified/Fsm.h"
#include "libraries/PinChangeInterrupt/src/PinChangeInterrupt.h"
#include "SPI.h"

//internal variables
tokenId_t curToken;
coffee_t curCoffee;

//Times
#define COFFEE_READY_WAIT_TIME 20000
#define AUTHENTICATED_TIME 5000
#define FAIL_TIME 10000

//Events - do not use 0!
#define AI_EVENT 2    // authenticated input
#define COB_EVENT 3   // coffee button
#define CAB_EVENT 4   // cancel button
#define UU_EVENT 5    // unknown user

State state_ready(String("ready"), &ready_entry, &ready_loop, NULL);
State state_authenticated(String("authenticated"), &authenticated_entry, NULL, &authenticated_exit);
State state_brewing(String("brewing_coffee"), &brewing_entry, NULL, NULL);

Fsm fsm_cm(&state_ready);

//-----------------------------------------------------------------------------------------------


void setup_statemachine() {
  logging("setup_statemachine");

  init_cm();
  init_interrupts();
  run_cm();
  ready_entry();
}




//---------------------------------------------------------------------------------------------------------------
void init_cm() {

  logging("init_cm");

/*
This is the current state machine. Visualisation, e.g. on https://dreampuf.github.io/GraphvizOnline/

digraph fsm_cm {

  rankdir = LR;

  ready;
  auth;
  brewing;

 
  ready -> auth [label = "AI", color = "black"];
  ready -> ready  [label = "UU", color = "black"];
  ready -> ready  [label = "10 sec", color = "blue"];

  auth -> ready [label = "CAB", color = "black"];
  auth -> brewing [label = "COB", color = "black"];
  auth -> ready [label = "5 sec", color = "blue"];

  brewing -> ready [label = "20 sec", color = "red"];
  brewing -> ready [label = "CAB", color = "black"];

}
 */

  //von ready
  fsm_cm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  fsm_cm.add_transition(&state_ready, &state_ready, UU_EVENT, &unknown_user);
  fsm_cm.add_timed_transition(&state_ready, &state_ready, FAIL_TIME, NULL);  // only to redisplay token msg

  //von authenticated
  fsm_cm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
  fsm_cm.add_transition(&state_authenticated, &state_brewing, COB_EVENT, NULL);
  fsm_cm.add_timed_transition(&state_authenticated, &state_ready, AUTHENTICATED_TIME, NULL);

  //von brewing
  fsm_cm.add_timed_transition(&state_brewing, &state_ready, COFFEE_READY_WAIT_TIME, &on_coffee_got);
  fsm_cm.add_transition(&state_brewing, &state_ready, CAB_EVENT, &on_no_coffee_got);

}

void init_interrupts() {
  logging("init_interrupts");

  //Pins
  pinMode(BUTTON_CANCEL, INPUT);
  pinMode(BUTTON_NORMAL, INPUT);
  pinMode(BUTTON_NORMAL_DOUBLE, INPUT);
  pinMode(BUTTON_ESPRESSO, INPUT);
  pinMode(BUTTON_ESPRESSO_DOUBLE, INPUT);

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
  logging(__FUNCTION__);
  gui.delayedPrint(String(" WAITING  ") + "FOR TOKEN"); 
}


void ready_loop() {
  // logging(__FUNCTION);

  tokenId_t token = checkForCard();
  
  if (NO_CARD == token)
    return;

  curToken = token;

  if (authenticateToken(token) == USER_UNKNOWN) {
    fsm_cm.trigger(UU_EVENT);
  } else {
    fsm_cm.trigger(AI_EVENT);
  }
}

void unknown_user() {
  logging(__FUNCTION__);
  gui.print(String("BAD TOKEN ") + curToken);
}

void authenticated_entry() {
  logging(__FUNCTION__);
  gui.print(String("GOOD TOKEN") + curToken);
}

void authenticated_exit() {
  logging(__FUNCTION__);
}


void brewing_entry() {
  logging(__FUNCTION__);
  brewCoffee(curCoffee);
  gui.print(String("  CANCEL   PROCESS? "));
}

void on_no_coffee_got() {
  logging(__FUNCTION__);
  gui.print(String(" CHARGING  NOTHING  "));
}

void on_coffee_got() {
  int8_t cost = 0;
  
  logging(__FUNCTION__);

  switch(curCoffee) {
    case BUTTON_NORMAL: 
    case BUTTON_ESPRESSO:
      cost = 1; 
      break;
    case BUTTON_NORMAL_DOUBLE:
    case BUTTON_ESPRESSO_DOUBLE:
      cost = 2;
      break; 
  }

  gui.print(String(" CHARGING  ") + cost);
  incrementCoffeeCount(curToken, cost);
  eepromStatus();
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
