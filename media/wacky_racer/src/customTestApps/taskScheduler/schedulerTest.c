/**
 @file schedulerTest.c
 @author Matthew King
 @date 25/04/2024
 @brief blinks LEDs at independant rates, inspired by RIOS
*/


#include "target.h"
#include <pio.h>
#include <pacer.h>
#include "../../apps/customLib/taskScheduler.h"

// Setup pacer, which will be used to control the task scheduler
#define TASK_SCHEDULER_RATE 100 // in HZ

// Declare the handler functions used in the task scheduler
void statusLedToggle(void) {
    pio_output_toggle(LED_STATUS_PIO);
}

void errorLedToggle(void) {
    pio_output_toggle(LED_ERROR_PIO);
}

void thirdLedToggle(void) {
    pio_output_toggle(LED3_PIO);
}

// Setup the task scheduler
task tasks[3]; 
const unsigned char tasksNum = 3;
const unsigned char periodStatus = 50; //scheduler rate / task period = task rate [Hz]
const unsigned char periodError = 100;
const unsigned char period3 = 200;

int main (void)
{
    // Assign properties to each task
    unsigned char i = 0;
    tasks[i].period = periodStatus; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &statusLedToggle;
    i++;
    tasks[i].period = periodError;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &errorLedToggle;
    i++;
    tasks[i].period = period3;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &thirdLedToggle;

    // Setup LEDs
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED3_PIO, PIO_OUTPUT_HIGH);

    // Start the pacer
    pacer_init(TASK_SCHEDULER_RATE * 2);

    // Main loop
    while (1){

        for (i=0; i < tasksNum; ++i) {
            if (tasks[i].elapsedTime >= tasks[i].period) { // If ready, 
                tasks[i].taskHandler(0); // Execute the task
                tasks[i].elapsedTime = 0; // reset it's countdown
            }
            // increment the task's counter
            tasks[i].elapsedTime += 1;
        }
        // Once each task has been iterated through, wait for the next pacer tick before starting again
        pacer_wait();

    };

}
