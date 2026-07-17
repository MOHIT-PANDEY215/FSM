#include "../include/queue.h"
#include <stdio.h>

// Initializes an empty queue
void Queue_Init(EventQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

// Pushes an event to the back of the queue
int Queue_Push(EventQueue *q, Event event) {
    if (q->count >= MAX_QUEUE_SIZE) {
        printf("Error: Event queue overflow! Cannot push event ID %d.\n", event.id);
        return 0; // Failure
    }
    
    q->events[q->tail] = event;
    // Circular increment for the tail index
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE; 
    q->count++;
    
    return 1; // Success
}

// Pops an event from the front of the queue
int Queue_Pop(EventQueue *q, Event *event) {
    if (Queue_IsEmpty(q)) {
        return 0; // Failure (queue is empty)
    }
    
    *event = q->events[q->head];
    // Circular increment for the head index
    q->head = (q->head + 1) % MAX_QUEUE_SIZE; 
    q->count--;
    
    return 1; // Success
}

// Checks if the queue has pending events
int Queue_IsEmpty(EventQueue *q) {
    return (q->count == 0);
}