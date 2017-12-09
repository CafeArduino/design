#include "BootingState.h"
#include <windows.h>


bool StateEngine::isInited = false;
StateEngine* StateEngine::instance = 0;

BootingState::BootingState()
{
    //ctor
}

void BootingState::entry() {
        Sleep(2000);
}

void BootingState::exit() {

}

char* BootingState::toString() {
    return "Booting_State";
}
