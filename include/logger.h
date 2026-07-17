#ifndef LOGGER_H
#define LOGGER_H

// Core logging functions
void Logger_LogTransition(int slot, const char *currentState, const char *eventName, const char *nextState, const char *actionName);
void Logger_LogEvent(int slot, const char *eventName);
void Logger_LogError(int slot, const char *errorMessage);

#endif 