#include <Fsm.h>


//Times
#define BOOTING_COMPLETED_TIME 5000
#define GOTO_STANDBY_TIME 10000
#define CANCEL_AUTHENTICATED_TIME 5000
#define BREWING_TIME 7000

//Events - do not use 0!
#define PW_EVENT 1
#define AI_EVENT 2
#define COB_EVENT 3
#define CAB_EVENT 4

void booting_entry();
void brewing_entry();

bool standby_sensor_state = true;
bool maintenance_sensor_state = false;

State state_standby(String("standby"), NULL, NULL, NULL);
State state_booting(String("booting"), &booting_entry, NULL, NULL);
State state_ready(String("ready"), NULL, NULL, NULL);
State state_authenticated(String("authenticated"), &authenticated_entry, NULL, &authenticated_exit);
State state_brewing(String("brewing"), &brewing_entry, NULL, NULL);
State state_needs_maintenance(String("needs_maintenance"), NULL, NULL, NULL);

Fsm fsm_cm(&state_standby);

//-----------------------------------------------------------------------------------------------
//state functions

void booting_entry() {
  //Trigger coffee machine booting
  triggerCM(CM_POWER);
}

void brewing_entry() {
  //Trigger coffee machine brewing
  triggerCM(CM_COFFEE);
}

void authenticated_entry() {
  digitalWrite(AUTHENTICATED_LED, HIGH);
}

void authenticated_exit() {
  digitalWrite(AUTHENTICATED_LED, LOW);
}

//---------------------------------------------------------------------------------------------------
//interrupt functions

void powerButtonPressed() {
  Serial.println("PW_EVENT triggered");
  fsm_cm.trigger(PW_EVENT);
}

/*void userAuthenticated() {
  Serial.println("AI_EVENT triggered");
  fsm_cm.trigger(AI_EVENT);
}*/

void coffeeButtonPressed() {
  Serial.println("COB_EVENT triggered");
  fsm_cm.trigger(COB_EVENT); 
}

void cancelButtonPressed() {
  Serial.println("CAB_EVENT triggered");
  fsm_cm.trigger(CAB_EVENT); 
}

void maintenanceSensorInput() {
  maintenance_sensor_state = !maintenance_sensor_state;
}

void standbySensorInput() {
  standby_sensor_state = !standby_sensor_state;
}

//-----------------------------------------------------------------------------------------------
//communication

void userAuthenticated() {
  fsm_cm.trigger(AI_EVENT);
}

//---------------------------------------------------------------------------------------------------------------
void init_cm() {
  //transitions

  //von standby
    fsm_cm.add_transition(&state_standby, &state_booting, PW_EVENT, NULL);
  //von booting
    //fsm_cm.add_condition_transition(&state_booting, &state_ready, &standby_sensor_state, false, NULL);     //Do not work properly with coffee machine model
    fsm_cm.add_timed_transition(&state_booting, &state_ready, BOOTING_COMPLETED_TIME, NULL);
  //von ready
    fsm_cm.add_condition_transition(&state_ready, &state_standby, &standby_sensor_state, true, NULL);
    //fsm_cm.add_transition(&state_ready, &state_standby, PW_EVENT, NULL);
    fsm_cm.add_condition_transition(&state_ready, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
    fsm_cm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  //von authenticated
    //fsm_cm.add_transition(&state_authenticated, &state_standby, PW_EVENT, NULL);
    fsm_cm.add_condition_transition(&state_authenticated, &state_standby, &standby_sensor_state, true, NULL);
    fsm_cm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
    fsm_cm.add_transition(&state_authenticated, &state_brewing, COB_EVENT, NULL);
    fsm_cm.add_condition_transition(&state_authenticated, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
  //von brewing
    fsm_cm.add_timed_transition(&state_brewing, &state_ready, BREWING_TIME, NULL);
    fsm_cm.add_condition_transition(&state_brewing, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
  //von needs_maintenance
    fsm_cm.add_condition_transition(&state_needs_maintenance, &state_ready, &maintenance_sensor_state, false, NULL);
    //fsm_cm.add_transition(&state_needs_maintenance, &state_standby, PW_EVENT, NULL);
    fsm_cm.add_condition_transition(&state_needs_maintenance, &state_standby, &standby_sensor_state, true, NULL);
}

void run_cm() {
  fsm_cm.run_machine();
}

//---------------------------------------------------------------------------------------------------------------
//util
void triggerCM(int pin)  {
  digitalWrite(pin, LOW);
  addTimedWrite(millis() + 500, pin, HIGH);
  Serial.print("Triggering pin ");
  Serial.println(pin);
}
