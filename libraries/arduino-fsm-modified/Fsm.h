// This file is part of arduino-fsm.
//
// arduino-fsm is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// arduino-fsm is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with arduino-fsm.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FSM_H
#define FSM_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif


struct State
{
  State(String name, void (*on_enter)(), void (*on_state)(), void (*on_exit)());
  void (*on_enter)();
  void (*on_state)();
  void (*on_exit)();
  String name;
};


class Fsm
{
public:
  Fsm(State* initial_state);
  ~Fsm();

  void add_transition(State* state_from, State* state_to, int event,
                      void (*on_transition)());

  void add_timed_transition(State* state_from, State* state_to,
                            unsigned long interval, void (*on_transition)());

  void add_condition_transition(State* state_from, State* state_to, bool* condition, bool cmp_val, void (*on_transition) ());

  void check_timed_transitions();
  void check_condition_transitions();

  void trigger(int event);
  void run_machine();

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    void (*on_transition)();

  };
  struct TimedTransition
  {
    Transition transition;
    unsigned long start;
    unsigned long interval;
  };
  struct ConditionTransition
  {
    Transition transition;
    bool* condition;
    bool cmp_val;
  };

  static Transition create_transition(State* state_from, State* state_to,
                                      int event, void (*on_transition)());

  void make_transition(Transition* transition);

private:
  State* m_current_state;
  Transition* m_transitions;
  int m_num_transitions;

  TimedTransition* m_timed_transitions;
  int m_num_timed_transitions;
  ConditionTransition* m_condition_transitions;
  int m_num_condition_transitions;
  bool m_initialized;
};


#endif
