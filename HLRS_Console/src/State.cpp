#include "State.h"

State::State(): coffeeMachine(new CoffeeMachine())
{
    //ctor
}

State::~State()
{
    //dtor
}

void State::setTimeout(int millis, void (*timeout)()) {
    //TODO: Implement timeout
}

void State::entry() {}

void State::exit() {}

void State::powerButtonPressed() {}

char* State::toString() {return "State_Base_Class";}
