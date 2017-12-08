
#include <iostream>
#include "CoffeeMachine.h"


CoffeeMachine::CoffeeMachine()
{
    //ctor
}

void CoffeeMachine::setPowerOutput(bool state) {
    std::cout<<"Power Output: " << state;
}
