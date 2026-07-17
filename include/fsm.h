#ifndef FSM_H
#define FSM_H

#include "state.h"
#include "event.h"
#include "transition.h"

// Define maximum limits to keep memory allocation simple for the demo
#define MAX_STATES 20
#define MAX_TRANSITIONS 50

typedef struct {
    void *context; // Pointer to specific protocol data (e.g., HARQ process data)
    
    int currentStateId;
    
    State states[MAX_STATES];
    int stateCount;
    
    Transition transitions[MAX_TRANSITIONS];
    int transitionCount;
} FSM_Instance;

// Functional APIs as per your specification
void FSM_Init(FSM_Instance *fsm, void *context);
void FSM_AddState(FSM_Instance *fsm, State state);
void FSM_AddTransition(FSM_Instance *fsm, Transition transition);
void FSM_ProcessEvent(FSM_Instance *fsm, Event *event);
void FSM_Reset(FSM_Instance *fsm, int initialStateId);

#endif