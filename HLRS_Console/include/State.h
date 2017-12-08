#ifndef STATE_H
#define STATE_H


class State
{
    public:
        State();
        virtual ~State();

        virtual void entry();
        virtual void exit();
        virtual void during();

        void setTimeout(int millis, void (*timeout)());

        virtual void powerButtonPressed();

    protected:

    private:
};

#endif // STATE_H
