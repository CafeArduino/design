#include "StandbyState.h"
#include <windows.h>

StandbyState::StandbyState()
{
    //ctor
}

void StandbyState::powerButtonPressed() {
    StateEngine::getInstance()->changeState(new BootingState());
}

void StandbyState::entry() {
    Sleep(2000);
    powerButtonPressed();
}

void StandbyState::exit() {}

char* StandbyState::toString() {
    return "Standby_State";
}
