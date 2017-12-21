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
#define MAINTENANCE_SENSOR 8
#define STANDBY_SENSOR 9

//Events
#define PW_EVENT 0
#define AI_EVENT 1
#define COB_EVENT 2
#define CAB_EVENT 3

bool standby_sensor_state = false;
bool maintenance_sensor_state = false;

State state_standby(NULL, NULL, NULL);
State state_booting(&booting_entry, NULL, NULL);
State state_ready(NULL, NULL, NULL);
State state_authenticated(NULL, NULL, NULL);
State state_brewing(&brewing_entry, NULL, NULL);
State state_needs_maintenance(NULL, NULL, NULL);

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

void 

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

void maintenanceSensorInput() {
  maintenance_sensor_state = !maintenance_sensor_state;
}

void standbySensorInput() {
  standby_sensor_state = !standby_sensor_state;
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
  fsm.add_transition(&state_booting, &state_ready, SS_f_EVENT, NULL);
  //von ready
  fsm.add_transition(&state_ready, &state_standby, SS_t_EVENT, NULL);
  fsm.add_transition(&state_ready, &state_standby, PW_EVENT, NULL);
  fsm.add_transition(&state_ready, &state_needs_maintenance, MS_t_EVENT, NULL);
  fsm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  //von authenticated
  fsm.add_transition(&state_authenticated, &state_standby, PW_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_standby, SS_t_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_brewing, COB_EVENT, NULL);
  fsm.add_transition(&state_authenticated, &state_needs_maintenance, MS_t_EVENT, NULL);
  //von brewing
  fsm.add_timed_transition(&state_brewing, &state_ready, BREWING_TIME, NULL);
  fsm.add_transition(&state_brewing, &state_needs_maintenance, MS_t_EVENT, NULL);
  //von needs_maintenance
  fsm.add_transition(&state_needs_maintenance, &state_ready, MS_f_EVENT, NULL);
  fsm.add_transition(&state_needs_maintenance, &state_standby, PW_EVENT, NULL);
  fsm.add_transition(&state_needs_maintenance, &state_standby, SS_t_EVENT, NULL);

  //Pins
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(CM_POWER, OUTPUT);
  pinMode(COFFEE_BUTTON, INPUT_PULLUP);
  pinMode(CM_COFFEE, OUTPUT);
  pinMode(CANCEL_BUTTON, INPUT_PULLUP);
  pinMode(AUTHENTICATED_BUTTON, INPUT_PULLUP);
  pinMode(MAINTENANCE_SENSOR, INPUT);
  pinMode(STANDBY_SENSOR, INPUT);

  //interrupts
  attachPCINT(digitalPinToPCINT(POWER_BUTTON), powerButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(AUTHENTICATED_BUTTON), userAuthenticated, FALLING);
  attachPCINT(digitalPinToPCINT(COFFEE_BUTTON), coffeeButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(CANCEL_BUTTON), cancelButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(MAINTENANCE_SENSOR), maintenanceSensorFalse, FALLING);
  attachPCINT(digitalPinToPCINT(MAINTENANCE_SENSOR), maintenanceSensorTrue, RISING);
  attachPCINT(digitalPinToPCINT(STANDBY_SENSOR), standbySensorFalse, FALLING);
  attachPCINT(digitalPinToPCINT(STANDBY_SENSOR), standbySensorTrue, RISING);
}

void loop() {
  fsm.run_machine();
  delay(100);
}
