#ifndef TIMER_H
#define TIMER_H

typedef struct {
    int isActive;
    int remainingSlots;
    int timeoutEventId; // The event to trigger when the timer hits zero
} Timer;

// Core timer functions based on your specification
void Timer_Init(Timer *t);
void Timer_Start(Timer *t, int slots, int eventId);
void Timer_Stop(Timer *t);
int Timer_Update(Timer *t); // Returns the eventId if a timeout occurs, or -1 otherwise

#endif