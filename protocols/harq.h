#ifndef HARQ_H
#define HARQ_H

#include "../include/fsm.h"
#include "../include/timer.h"

#define HARQ_MAX_RETRIES 3
#define HARQ_TIMEOUT_SLOTS 4

// HARQ Specific States
typedef enum {
    HARQ_STATE_IDLE = 0,
    HARQ_STATE_NEW_TX = 1,
    HARQ_STATE_WAITING_ACK = 2,
    HARQ_STATE_RETX = 3,
    HARQ_STATE_COMPLETED = 4
} HARQ_StateId;

// HARQ Specific Events
typedef enum {
    HARQ_EVENT_SCHEDULE = 10,
    HARQ_EVENT_PACKET_SENT = 11,
    HARQ_EVENT_ACK = 12,
    HARQ_EVENT_NACK = 13,
    HARQ_EVENT_TIMEOUT = 14,
    HARQ_EVENT_MAX_RETRY = 15
} HARQ_EventId;

// Protocol-specific context to hold data during execution
typedef struct {
    int processId;
    int retryCount;
    Timer *timer; // Pointer to the timer module for timeouts
} HARQ_Context;

// Public API to configure the FSM
void HARQ_Setup(FSM_Instance *fsm, HARQ_Context *ctx);

#endif 