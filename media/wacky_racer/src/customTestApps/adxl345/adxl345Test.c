/**
 @file adxl345Test.c
 @author Matthew King
 @date 25/04/2024
 @brief Checks the Adxl345 works with the task schedulers
*/


#include "target.h"
#include <pio.h>
#include <pacer.h>
#include "adxl345.h"
#include "usb_serial.h"
#include "panic.h"
#include "../../apps/customLib/taskScheduler.h"

// Setup pacer, which will be used to control the task scheduler
#define TASK_SCHEDULER_RATE 100 // in HZ

/* Setup the accelerometer */
static twi_cfg_t adxl345_twi_cfg =
{
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR (100000), // 100 kHz
    .slave_addr = 0
};


/* Declare the handler functions used in the task scheduler */
int statusLedToggle(int); 
int pollAccelerometer(int);

int16_t accel[3]; // Acelerometer values

// Setup the task scheduler
task tasks[2];
const unsigned char tasksNum = 2;
const unsigned char periodStatus = 100; // Run once per second
const unsigned char periodAccel = 10; // Poll 10 times per second

int main (void)
{
    unsigned char i = 0;
    tasks[i].period = periodStatus; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &statusLedToggle;
    tasks[i].arg = 0; // not used for this func but we have to assign it
    i++;
    tasks[i].period = periodAccel;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &pollAccelerometer;
    tasks[i].arg = 0; // set to initialise

    // Redirect stdio to usb serial
    usb_serial_stdio_init();

    // Setup LEDs
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);

    // Start the pacer
    pacer_init(TASK_SCHEDULER_RATE * 2);

    // Main loop
    while (1){

        for (i=0; i < tasksNum; ++i) {
            if (tasks[i].elapsedTime >= tasks[i].period) { // If ready, 
                tasks[i].taskHandler(tasks[i].arg); // Execute the task
                tasks[i].elapsedTime = 0; // reset it's countdown
            }
            // increment the task's counter
            tasks[i].elapsedTime += 1;
        }
        // Once each task has been iterated through, wait for the next pacer tick before starting again
        pacer_wait();

    };
}

int statusLedToggle(int arg) {
    pio_output_toggle(LED_STATUS_PIO);
    return 1;
}

int pollAccelerometer(int state) {
    static twi_t adxl345_twi;  
    static adxl345_t *adxl345;
    // Implements a FSM so first function call initialises the ADXL
    switch(state) {
        case 0: // Initialise the adxl
            // Setup the I2C
            adxl345_twi = twi_init(&adxl345_twi_cfg);
            // Panic if it fails
            if (! adxl345_twi) {
                panic (LED_ERROR_PIO, 1);
            }
            // Setup the adxl
            adxl345 = adxl345_init(adxl345_twi, ADXL345_ADDRESS);
            // Panic if it fails
            if (! adxl345) {
                panic(LED_ERROR_PIO, 2);
            }
            // No need to break, happy to sample straight after init
            state = 1;
        case 1:
            if (adxl345_is_ready(adxl345)) {   // Check if the adxl is ready
                if (adxl345_accel_read(adxl345, accel)) { // Sample the adxl
                    printf ("x: %5d  y: %5d  z: %5d\n", accel[0], accel[1], accel[2]);
                } else {
                    printf ("ERROR: failed to read acceleration\n");
                }
            } else {
                printf ("ERROR: failed to read acceleration\n");
                break;
            }
    } 
    return state;      
}