#ifndef EVENT_H
#define EVENT_H

//this defines the skeleton for the trigger which causes transitions
typedef struct {
    int id;
    int slot;
    int ue_id;
    int harq_process_id;
    void *payload;
} Event;

#endif 