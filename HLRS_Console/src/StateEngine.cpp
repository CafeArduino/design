#include "StateEngine.h"
#include<iostream>



StateEngine::StateEngine(State* startState): currentState(startState)
{
}

void StateEngine::init(State* startState) {
    isInited = true;
    instance = new StateEngine(startState);
    instance->start();
}

StateEngine* StateEngine::getInstance() {
    if(!isInited) {throw "state_engine_not_inited";}

    return instance;
}

void StateEngine::changeState(State *newState) {
    currentState->exit();
    //delete currentState;
    std::cout<<"Changing to: " << newState->toString()<<"\n";
    currentState = newState;
    currentState->entry();
}

void StateEngine::start() {
    std::cout<<"Changing to: " << currentState->toString()<<"\n";
    currentState->entry();
}
