#ifndef STATE_H
#define STATE_H

//this file is the default skeleton for the functions which trigger when a state is updated
typedef struct {
    int id;
    const char *name;

    void (*onEnter)(void *context);
    void (*onExit)(void *context);
    void (*onUpdate)(void *context);
} State;

#endif