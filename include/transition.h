#ifndef TRANSITION_H
#define TRANSITION_H

//this defines the rule to transition from one state to another

typedef struct {
    int currentState;
    int event;
    int nextState;

    int (*guard)(void *context);
    void (*action)(void *context);
} Transition;

#endif 