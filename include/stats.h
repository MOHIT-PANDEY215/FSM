#ifndef STATS_H
#define STATS_H

#define MAX_TRACKED_STATES 20

// Defines what metric is being updated
typedef enum {
    STAT_TRANSITION_VALID,
    STAT_TRANSITION_INVALID,
    STAT_TIMEOUT,
    STAT_RETRY,
    STAT_STATE_VISIT
} StatType;

typedef struct {
    int totalTransitions;
    int invalidTransitions;
    int timeoutCount;
    int retryCount;
    int currentStateId;
    int stateVisits[MAX_TRACKED_STATES];
} FSM_Stats;

// Core statistics functions mapped to your specification
void Stats_Init(FSM_Stats *stats);
void Stats_Update(FSM_Stats *stats, StatType type, int value);
void Stats_Print(const FSM_Stats *stats);

#endif 