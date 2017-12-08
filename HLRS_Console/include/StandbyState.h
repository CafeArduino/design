#ifndef STANDBY_STATE_H
#define STANDBY_STATE_H

#include "State.h"
#include "BootingState.h"

class StandbyState : public State {
    public:
        StandbyState();

        char* toString();

        void powerButtonPressed();
        void entry();
        void exit();

    protected:

    private:
};

#endif // STANDBY_STATE_H
