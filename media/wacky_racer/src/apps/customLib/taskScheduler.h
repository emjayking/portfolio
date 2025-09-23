/**
 @file taskScheduler.h
 @author Matthew King
 @date 25/04/2024
 @brief A standard, non pre-emptive timer based task scheduler inspired by RIOS
*/

/*
Notes
- see "customTestLib" folder for sample code
- Designed such that a task should be finished before the next pacer tick
- There is no watchdog for if a task does overrun a pacer-tick
- Tasks at the begining of the list will be completed first
- Slower pacer rates give more time for tasks to be completed, but limit how often tasks can occur
*/

#ifndef TASK_SCHEDULER
#define TASK_SCHEDULER



typedef struct task
{
    unsigned long period;           // Time between function calls
    unsigned long elapsedTime;      // Time since last call
    int arg;                        // An argument you can pass to the the function to act as an fsm or something
    int (*taskHandler)(int);        // Function to call, must pass in arg
} task;



#endif