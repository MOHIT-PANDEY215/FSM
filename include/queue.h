#ifndef QUEUE_H
#define QUEUE_H

#include "event.h"

// Define a maximum size to prevent memory exhaustion
#define MAX_QUEUE_SIZE 100

typedef struct {
    Event events[MAX_QUEUE_SIZE];
    int head;  // Index to pop from
    int tail;  // Index to push to
    int count; // Current number of events in the queue
} EventQueue;

// Core queue functions 
void Queue_Init(EventQueue *q);
int Queue_Push(EventQueue *q, Event event);
int Queue_Pop(EventQueue *q, Event *event);
int Queue_IsEmpty(EventQueue *q);

#endif