#ifndef STATEENGINE_H
#define STATEENGINE_H

#include "State.h"
class StandbyState;
#include <iostream>

class State;

class StateEngine
{
    public:
        void changeState(State *newState);
        static void init(State *startState);
        static StateEngine* getInstance();
    protected:

    private:
        StateEngine(State *startState);
        State *currentState;
        static StateEngine *instance;
        static bool isInited;
        void start();
};


#endif // STATEENGINE_H
