#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

#include <Fsm.h>


//Times
#define BOOTING_COMPLETED_TIME 5000
#define GOTO_STANDBY_TIME 10000
#define CANCEL_AUTHENTICATED_TIME 5000
#define BREWING_TIME 5000

//Pins
#define POWER_BUTTON 2
#define CM_POWER 3
#define COFFEE_BUTTON 4
#define CM_COFFEE 5
#define CANCEL_BUTTON 6
#define AUTHENTICATED_BUTTON 7
#define MAINTENANCE_BUTTON 8
#define MAINTENANCE_LED 9

//Events
#define PW_EVENT 0
#define AI_EVENT 1
#define COB_EVENT 2
#define CAB_EVENT 3
#define MS_t_EVENT 4
#define MS_f_EVENT 5

State state_standby(NULL, NULL, NULL);
State state_booting(&booting_entry, NULL, NULL);
State state_ready(NULL, NULL, NULL);
State state_authenticated(NULL, NULL, NULL);
State state_brewing(&brewing_entry, NULL, NULL);
State state_needs_maintenance(&needs_maintenance_entry, NULL, &needs_maintenance_exit);

Fsm fsm(&state_standby);

//-----------------------------------------------------------------------------------------------
//state functions

void booting_entry() {
  //Trigger coffee machine booting
  digitalWrite(CM_POWER, HIGH);
  delay(500);
  digitalWrite(CM_POWER, LOW);
}

void brewing_entry() {
  //Trigger coffee machine brewing
  digitalWrite(CM_COFFEE, HIGH);
  delay(500);
  digitalWrite(CM_COFFEE, LOW);  
}

void needs_maintenance_entry() {
 digitalWrite(MAINTENANCE_LED, HIGH);
}

void needs_maintenance_exit() {
 digitalWrite(MAINTENANCE_LED, LOW);
}

//---------------------------------------------------------------------------------------------------
//interrupt functions

void powerButtonPressed() {
  Serial.println("PW_EVENT triggered");
  fsm.trigger(PW_EVENT);
}

void userAuthenticated() {
  Serial.println("AI_EVENT triggered");
  fsm.trigger(AI_EVENT);
}

void coffeeButtonPressed() {
  Serial.println("COB_EVENT triggered");
  fsm.trigger(COB_EVENT); 
}

void cancelButtonPressed() {
  Serial.println("CAB_EVENT triggered");
  fsm.trigger(CAB_EVENT); 
}

bool msSensorState = false;
void maintenanceInput() {
  msSensorState = !msSensorState;
  if(msSensorState) {
    msSensorState = 0;
    Serial.println("MS_t_EVENT triggered");
    fsm.trigger(MS_t_EVENT);
  } else {
    Serial.println("MS_f_EVENT triggered");
    fsm.trigger(MS_f_EVENT);
  }
}

//---------------------------------------------------------------------------------------------------------------
//arduino functions
void setup() {
  Serial.begin(9600);
  pinMode(POWER_BUTTON, INPUT);
  pinMode(CM_POWER, OUTPUT);

  //transitions

  //von standby
  fsm.add_transition(&state_standby, &state_booting, PW_EVENT, NULL);
  //von booting
  fsm.add_timed_transition(&state_booting, &state_ready, BOOTING_COMPLETED_TIME, NULL);
  //von ready
  fsm.add_timed_transition(&state_ready, &state_standby, GOTO_STANDBY_TIME, NULL);
  fsm.add_transition(&state_ready, &state_standby, PW_EVENT, NULL);
  fsm.add_transition(&state_ready, &state_needs_maintenance, MS_t_EVENT, NULL);
  fsm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  //von authenticated
  fsm.add_transition(&state_authenticated, &state_standby, PW_EVENT, NULL);
  fsm.add_timed_transition(&state_authenticated, &state_standby, GOTO_STANDBY_TIME, NULL);
  fsm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_brewing, COB_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_needs_maintenance, MS_t_EVENT, NULL);
  //von brewing
  fsm.add_timed_transition(&state_brewing, &state_ready, BREWING_TIME, NULL);
  fsm.add_transition(&state_brewing, &state_needs_maintenance, MS_t_EVENT, NULL);
  //von needs_maintenance
  fsm.add_transition(&state_needs_maintenance, &state_ready, MS_f_EVENT, NULL);
  fsm.add_transition(&state_needs_maintenance, &state_standby, PW_EVENT, NULL);
  fsm.add_timed_transition(&state_needs_maintenance, &state_standby, GOTO_STANDBY_TIME, NULL);



  //interrupts
  attachPCINT(digitalPinToPCINT(POWER_BUTTON), powerButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(AUTHENTICATED_BUTTON), userAuthenticated, FALLING);
  attachPCINT(digitalPinToPCINT(COFFEE_BUTTON), coffeeButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(CANCEL_BUTTON), cancelButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(MAINTENANCE_BUTTON), maintenanceInput, FALLING);
}

void loop() {
  fsm.run_machine();
  delay(100);
}
