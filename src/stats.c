#include "../include/stats.h"
#include <stdio.h>

// Initializes all statistical counters to zero
void Stats_Init(FSM_Stats *stats) {
    stats->totalTransitions = 0;
    stats->invalidTransitions = 0;
    stats->timeoutCount = 0;
    stats->retryCount = 0;
    stats->currentStateId = -1;
    
    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        stats->stateVisits[i] = 0;
    }
}

// Updates the specified metric
void Stats_Update(FSM_Stats *stats, StatType type, int value) {
    switch (type) {
        case STAT_TRANSITION_VALID:
            stats->totalTransitions++;
            break;
            
        case STAT_TRANSITION_INVALID:
            stats->invalidTransitions++;
            break;
            
        case STAT_TIMEOUT:
            stats->timeoutCount++;
            break;
            
        case STAT_RETRY:
            stats->retryCount++;
            break;
            
        case STAT_STATE_VISIT:
            stats->currentStateId = value;
            if (value >= 0 && value < MAX_TRACKED_STATES) {
                stats->stateVisits[value]++;
            }
            break;
    }
}

// Prints the formatted final statistics
void Stats_Print(const FSM_Stats *stats) {
    printf("\n========================================\n");
    printf("         FSM EXECUTION STATISTICS       \n");
    printf("========================================\n");
    printf("Total Transitions    : %d\n", stats->totalTransitions);
    printf("Invalid Transitions  : %d\n", stats->invalidTransitions);
    printf("Timeout Count        : %d\n", stats->timeoutCount);
    printf("Retry Count          : %d\n", stats->retryCount);
    printf("Final Current State  : %d\n", stats->currentStateId);
    
    printf("\nState Visit Breakdown:\n");
    for (int i = 0; i < MAX_TRACKED_STATES; i++) {
        if (stats->stateVisits[i] > 0) {
            printf("  State [%d] Visits   : %d\n", i, stats->stateVisits[i]);
        }
    }
    printf("========================================\n\n");
}