#include "../include/timer.h"

// Initialize the timer state
void Timer_Init(Timer *t) {
    t->isActive = 0;
    t->remainingSlots = 0;
    t->timeoutEventId = -1;
}

// Start the timer with a specific duration and target event
void Timer_Start(Timer *t, int slots, int eventId) {
    if (slots > 0) {
        t->isActive = 1;
        t->remainingSlots = slots;
        t->timeoutEventId = eventId;
    }
}

// Manually stop the timer (e.g., if an ACK is received before timeout)
void Timer_Stop(Timer *t) {
    t->isActive = 0;
    t->remainingSlots = 0;
}

// Decrement the timer. Returns the timeout event ID if it expires, else -1.
int Timer_Update(Timer *t) {
    if (t->isActive) {
        t->remainingSlots--;
        
        if (t->remainingSlots <= 0) {
            t->isActive = 0; // Timer expired
            return t->timeoutEventId;
        }
    }
    return -1; // Timer still running or inactive
}