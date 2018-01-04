#include <Fsm.h>

#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>


//Times
#define BOOTING_COMPLETED_TIME 5000
#define GOTO_STANDBY_TIME 10000
#define CANCEL_AUTHENTICATED_TIME 5000
#define BREWING_TIME 7000

//Pins
#define POWER_BUTTON 2
#define CM_POWER 3
#define COFFEE_BUTTON 4
#define CM_COFFEE 5
#define CANCEL_BUTTON 6
#define AUTHENTICATED_BUTTON 7
#define MAINTENANCE_SENSOR 8
#define STANDBY_SENSOR 9

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
State state_authenticated(String("authenticated"), NULL, NULL, NULL);
State state_brewing(String("brewing"), &brewing_entry, NULL, NULL);
State state_needs_maintenance(String("needs_maintenance"), NULL, NULL, NULL);

Fsm fsm(&state_standby);

//-----------------------------------------------------------------------------------------------
struct TimedWrite {
  unsigned long time;
  int pin;
  bool value;
};

TimedWrite* timed_writes;
int num_timed_writes = 0;

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
//util
void triggerCM(int pin)  {
  digitalWrite(pin, LOW);
  addTimedWrite(millis() + 500, pin, HIGH);
  Serial.print("Triggering pin ");
  Serial.println(pin);
}

void addTimedWrite(long time, int pin, bool value) {
  TimedWrite t_write;
  t_write.time = time;
  t_write.pin = pin;
  t_write.value = value;
  timed_writes = (TimedWrite*) realloc(timed_writes, (num_timed_writes + 1) * sizeof(TimedWrite));
  timed_writes[num_timed_writes] = t_write;
  num_timed_writes++;
}
//---------------------------------------------------------------------------------------------------------------
//arduino functions
void setup() {
  Serial.begin(9600);

  //transitions

  //von standby
    fsm.add_transition(&state_standby, &state_booting, PW_EVENT, NULL);
  //von booting
    //fsm.add_condition_transition(&state_booting, &state_ready, &standby_sensor_state, false, NULL);     //Do not work properly with coffee machine model
    fsm.add_timed_transition(&state_booting, &state_ready, BOOTING_COMPLETED_TIME, NULL);
  //von ready
    fsm.add_condition_transition(&state_ready, &state_standby, &standby_sensor_state, true, NULL);
    //fsm.add_transition(&state_ready, &state_standby, PW_EVENT, NULL);
    fsm.add_condition_transition(&state_ready, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
    fsm.add_transition(&state_ready, &state_authenticated, AI_EVENT, NULL);
  //von authenticated
    //fsm.add_transition(&state_authenticated, &state_standby, PW_EVENT, NULL);
    fsm.add_condition_transition(&state_authenticated, &state_standby, &standby_sensor_state, true, NULL);
    fsm.add_transition(&state_authenticated, &state_ready, CAB_EVENT, NULL);
    fsm.add_transition(&state_authenticated, &state_brewing, COB_EVENT, NULL);
    fsm.add_condition_transition(&state_authenticated, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
  //von brewing
    fsm.add_timed_transition(&state_brewing, &state_ready, BREWING_TIME, NULL);
    fsm.add_condition_transition(&state_brewing, &state_needs_maintenance, &maintenance_sensor_state, true, NULL);
  //von needs_maintenance
    fsm.add_condition_transition(&state_needs_maintenance, &state_ready, &maintenance_sensor_state, false, NULL);
    //fsm.add_transition(&state_needs_maintenance, &state_standby, PW_EVENT, NULL);
    fsm.add_condition_transition(&state_needs_maintenance, &state_standby, &standby_sensor_state, true, NULL);

  //Pins
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(CM_POWER, OUTPUT);
  digitalWrite(CM_POWER, HIGH);
  pinMode(COFFEE_BUTTON, INPUT_PULLUP);
  pinMode(CM_COFFEE, OUTPUT);
  digitalWrite(CM_COFFEE, HIGH);
  pinMode(CANCEL_BUTTON, INPUT_PULLUP);
  pinMode(AUTHENTICATED_BUTTON, INPUT_PULLUP);
  pinMode(MAINTENANCE_SENSOR, INPUT);
  pinMode(STANDBY_SENSOR, INPUT);

  //interrupts
  attachPCINT(digitalPinToPCINT(POWER_BUTTON), powerButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(AUTHENTICATED_BUTTON), userAuthenticated, FALLING);
  attachPCINT(digitalPinToPCINT(COFFEE_BUTTON), coffeeButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(CANCEL_BUTTON), cancelButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(MAINTENANCE_SENSOR), maintenanceSensorInput, CHANGE);
  attachPCINT(digitalPinToPCINT(STANDBY_SENSOR), standbySensorInput, CHANGE);
}

void loop() {
  fsm.run_machine();
  for(int i=0; i<num_timed_writes; i++) {
    if((timed_writes[i].time > (millis()-(unsigned int)50)) && (timed_writes[i].time < (millis()+(unsigned int)50))) {
      digitalWrite(timed_writes[i].pin, timed_writes[i].value);
    }
  }
  delay(50);
}
