#include "../include/logger.h"
#include <stdio.h>

// Logs a complete state transition matching the required format
void Logger_LogTransition(int slot, const char *currentState, const char *eventName, const char *nextState, const char *actionName) {
    printf("\nSlot %d\n\n", slot);
    printf("Current : %s\n", currentState ? currentState : "Unknown");
    printf("Event   : %s\n", eventName ? eventName : "Unknown");
    printf("Next    : %s\n", nextState ? nextState : "Unknown");
    printf("Action  : %s\n", actionName ? actionName : "None");
    printf("----------------------------------------\n");
}

// Helper to log when a new event is generated and pushed to the queue
void Logger_LogEvent(int slot, const char *eventName) {
    printf("[Slot %d] Event Generated: %s\n", slot, eventName ? eventName : "Unknown");
}

// Helper to log system errors (e.g., unhandled events, queue overflows)
void Logger_LogError(int slot, const char *errorMessage) {
    printf("[Slot %d] ERROR: %s\n", slot, errorMessage ? errorMessage : "Unknown Error");
}