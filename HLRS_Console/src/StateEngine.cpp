#include "StateEngine.h"
#include<iostream>


bool StateEngine::isInited = false;
StateEngine* StateEngine::instance = 0;

StateEngine::StateEngine(State* startState): currentState(startState)
{
}

void StateEngine::init(State* startState) {
    instance = new StateEngine(startState);
    isInited = true;
}

StateEngine* StateEngine::getInstance() {
    if(!isInited) throw "state_enigine_not_inited";

    return instance;
}

void StateEngine::changeState(State *newState) {
    currentState->exit();
    delete currentState;
    currentState = newState;
    currentState->entry();
    std::cout<<"Changing to: " << newState->toString();
}
