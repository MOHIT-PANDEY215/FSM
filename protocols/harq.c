#include "harq.h"
#include <stdio.h>
#include <stddef.h>

// ==========================================
// Action Callbacks (as per specification)
// ==========================================

void HARQ_Action_Allocate(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    ctx->retryCount = 0;
    // Logically allocate process here
}

void HARQ_Action_StartTimer(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    Timer_Start(ctx->timer, HARQ_TIMEOUT_SLOTS, HARQ_EVENT_TIMEOUT);
}

void HARQ_Action_StopTimer(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    Timer_Stop(ctx->timer);
}

void HARQ_Action_IncrementRetry(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    ctx->retryCount++;
}

void HARQ_Action_ReleaseProcess(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    ctx->retryCount = 0;
    // Logically release process to the scheduler
}

void HARQ_Action_NotifyFailure(void *context) {
    // Notify higher layers that the transmission failed
}

// ==========================================
// Guard Conditions
// ==========================================

// Guard to check if we are allowed to retry
int HARQ_Guard_CanRetry(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    return (ctx->retryCount < HARQ_MAX_RETRIES);
}

// Guard to trigger failure if retries exceeded
int HARQ_Guard_MaxRetryReached(void *context) {
    HARQ_Context *ctx = (HARQ_Context *)context;
    return (ctx->retryCount >= HARQ_MAX_RETRIES);
}

// ==========================================
// Setup Function: Wiring it to the Engine
// ==========================================

void HARQ_Setup(FSM_Instance *fsm, HARQ_Context *ctx) {
    FSM_Init(fsm, ctx);

    // 1. Register States
    FSM_AddState(fsm, (State){ HARQ_STATE_IDLE, "Idle", NULL, NULL, NULL });
    FSM_AddState(fsm, (State){ HARQ_STATE_NEW_TX, "NewTransmission", NULL, NULL, NULL });
    FSM_AddState(fsm, (State){ HARQ_STATE_WAITING_ACK, "WaitingACK", NULL, NULL, NULL });
    FSM_AddState(fsm, (State){ HARQ_STATE_RETX, "Retransmission", NULL, NULL, NULL });
    FSM_AddState(fsm, (State){ HARQ_STATE_COMPLETED, "Completed", HARQ_Action_ReleaseProcess, NULL, NULL });

    // 2. Register Transitions
    // From Idle
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_IDLE, HARQ_EVENT_SCHEDULE, HARQ_STATE_NEW_TX, NULL, HARQ_Action_Allocate });

    // From NewTransmission
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_NEW_TX, HARQ_EVENT_PACKET_SENT, HARQ_STATE_WAITING_ACK, NULL, HARQ_Action_StartTimer });

    // From WaitingACK - Success
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_WAITING_ACK, HARQ_EVENT_ACK, HARQ_STATE_COMPLETED, NULL, HARQ_Action_StopTimer });
    
    // From WaitingACK - NACK or Timeout (Retry allowed)
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_WAITING_ACK, HARQ_EVENT_NACK, HARQ_STATE_RETX, HARQ_Guard_CanRetry, HARQ_Action_IncrementRetry });
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_WAITING_ACK, HARQ_EVENT_TIMEOUT, HARQ_STATE_RETX, HARQ_Guard_CanRetry, HARQ_Action_IncrementRetry });

    // From WaitingACK - NACK or Timeout (Max Retries reached)
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_WAITING_ACK, HARQ_EVENT_NACK, HARQ_STATE_IDLE, HARQ_Guard_MaxRetryReached, HARQ_Action_NotifyFailure });
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_WAITING_ACK, HARQ_EVENT_TIMEOUT, HARQ_STATE_IDLE, HARQ_Guard_MaxRetryReached, HARQ_Action_NotifyFailure });

    // From Retransmission
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_RETX, HARQ_EVENT_PACKET_SENT, HARQ_STATE_WAITING_ACK, NULL, HARQ_Action_StartTimer });

    // From Completed -> Loop back to Idle
    FSM_AddTransition(fsm, (Transition){ HARQ_STATE_COMPLETED, HARQ_EVENT_SCHEDULE, HARQ_STATE_NEW_TX, NULL, HARQ_Action_Allocate });
}