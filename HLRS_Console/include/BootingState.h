#ifndef BOOTINGSTATE_H
#define BOOTINGSTATE_H

#include "State.h"

class BootingState : public State
{
    public:
        BootingState();

        void entry();
        void exit();

        char* toString();

    protected:

    private:
};

#endif // BOOTINGSTATE_H
