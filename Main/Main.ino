#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

//Pins
#define POWER_BUTTON 2
#define CM_POWER 3
#define COFFEE_BUTTON 4
#define CM_COFFEE 5
#define CANCEL_BUTTON 6
//#define AUTHENTICATED_BUTTON 7
#define MAINTENANCE_SENSOR 8
#define AUTHENTICATED_LED 14
#define STANDBY_SENSOR 15
//-----------------------------------------------------------------------------------------------
struct TimedWrite {
  unsigned long time;
  int pin;
  bool value;
};

TimedWrite* timed_writes;
int num_timed_writes = 0;

void addTimedWrite(long time, int pin, bool value) {
  TimedWrite t_write;
  t_write.time = time;
  t_write.pin = pin;
  t_write.value = value;
  timed_writes = (TimedWrite*) realloc(timed_writes, (num_timed_writes + 1) * sizeof(TimedWrite));
  timed_writes[num_timed_writes] = t_write;
  num_timed_writes++;
}

//-----------------------------------------------------------------------------------------------
//arduino functions

void setup() {
  Serial.begin(9600);

  //Pins
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(CM_POWER, OUTPUT);
  digitalWrite(CM_POWER, HIGH);
  pinMode(COFFEE_BUTTON, INPUT_PULLUP);
  pinMode(CM_COFFEE, OUTPUT);
  digitalWrite(CM_COFFEE, HIGH);
  pinMode(CANCEL_BUTTON, INPUT_PULLUP);
  //pinMode(AUTHENTICATED_BUTTON, INPUT_PULLUP);
  pinMode(MAINTENANCE_SENSOR, INPUT);
  pinMode(STANDBY_SENSOR, INPUT);
  pinMode(AUTHENTICATED_LED, OUTPUT);

  //interrupts
  attachPCINT(digitalPinToPCINT(POWER_BUTTON), powerButtonPressed, FALLING);
  //attachPCINT(digitalPinToPCINT(AUTHENTICATED_BUTTON), userAuthenticated, FALLING);
  attachPCINT(digitalPinToPCINT(COFFEE_BUTTON), coffeeButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(CANCEL_BUTTON), cancelButtonPressed, FALLING);
  attachPCINT(digitalPinToPCINT(MAINTENANCE_SENSOR), maintenanceSensorInput, CHANGE);
  attachPCINT(digitalPinToPCINT(STANDBY_SENSOR), standbySensorInput, CHANGE);

  init_cm();
  init_rfid();
}

void loop() {
  run_cm();
  checkForCard();
  for(int i=0; i<num_timed_writes; i++) {
    if((timed_writes[i].time > (millis()-(unsigned int)50)) && (timed_writes[i].time < (millis()+(unsigned int)50))) {
      digitalWrite(timed_writes[i].pin, timed_writes[i].value);
    }
  }
  delay(50);
}
