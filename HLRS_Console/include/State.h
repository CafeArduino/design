#ifndef STATE_H
#define STATE_H

#include "CoffeeMachine.h"

#include "StateEngine.h"

class State
{
    public:
        State();
        virtual ~State();

        virtual void entry();
        virtual void exit();

        virtual char* toString();

        void setTimeout(int millis, void (*timeout)());

        virtual void powerButtonPressed();

    protected:
        CoffeeMachine *coffeeMachine;

    private:
};

#endif // STATE_H
