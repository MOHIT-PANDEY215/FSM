#include "../include/fsm.h"
#include <stdio.h>
#include <stddef.h>

void FSM_Init(FSM_Instance *fsm, void *context) {
    fsm->context = context;
    fsm->stateCount = 0;
    fsm->transitionCount = 0;
    fsm->currentStateId = -1; // -1 represents an uninitialized state
}

void FSM_AddState(FSM_Instance *fsm, State state) {
    if (fsm->stateCount < MAX_STATES) {
        fsm->states[fsm->stateCount++] = state;
    } else {
        printf("Error: Maximum states reached.\n");
    }
}

void FSM_AddTransition(FSM_Instance *fsm, Transition transition) {
    if (fsm->transitionCount < MAX_TRANSITIONS) {
        fsm->transitions[fsm->transitionCount++] = transition;
    } else {
        printf("Error: Maximum transitions reached.\n");
    }
}

void FSM_Reset(FSM_Instance *fsm, int initialStateId) {
    fsm->currentStateId = initialStateId;
    
    // Trigger onEnter callback for the initial state if it exists
    for (int i = 0; i < fsm->stateCount; i++) {
        if (fsm->states[i].id == initialStateId) {
            if (fsm->states[i].onEnter != NULL) {
                fsm->states[i].onEnter(fsm->context);
            }
            break;
        }
    }
}

void FSM_ProcessEvent(FSM_Instance *fsm, Event *event) {
    int eventHandled = 0;

    // Transition lookup table logic
    for (int i = 0; i < fsm->transitionCount; i++) {
        Transition *t = &fsm->transitions[i];

        // Match current state and incoming event
        if (t->currentState == fsm->currentStateId && t->event == event->id) {
            
            // Execute guard; if it returns 0 (false), reject transition
            if (t->guard != NULL && !t->guard(fsm->context)) {
                continue; 
            }

            eventHandled = 1;

            // 1. Execute onExit for the outgoing state
            for (int j = 0; j < fsm->stateCount; j++) {
                if (fsm->states[j].id == fsm->currentStateId) {
                    if (fsm->states[j].onExit != NULL) {
                        fsm->states[j].onExit(fsm->context);
                    }
                    break;
                }
            }

            // 2. Execute the transition action
            if (t->action != NULL) {
                t->action(fsm->context);
            }

            // 3. Change the state
            fsm->currentStateId = t->nextState;

            // 4. Execute onEnter for the incoming state
            for (int j = 0; j < fsm->stateCount; j++) {
                if (fsm->states[j].id == fsm->currentStateId) {
                    if (fsm->states[j].onEnter != NULL) {
                        fsm->states[j].onEnter(fsm->context);
                    }
                    break;
                }
            }

            break; // Transition complete, exit the lookup loop
        }
    }

    // Fallback for unexpected events
    if (!eventHandled) {
        printf("Warning: Event %d was unhandled in State %d\n", event->id, fsm->currentStateId);
    }
}