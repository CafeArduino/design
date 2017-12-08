#include <iostream>

#include "StateEngine.h"
#include "StandbyState.h"
#include "src/StandbyState.cpp"
#include "State.h"

int main()
{
    StateEngine::init(new StandbyState());
}
