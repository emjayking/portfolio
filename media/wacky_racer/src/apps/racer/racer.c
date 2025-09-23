/**
 @file hat.c
 @author Matthew King
 @date 25/04/2024
 @brief main operating system for hat PCB
*/
#include "nrf24.h"
#include "spi.h"
#include "target.h"
#include "pio.h"
#include "usb_serial.h"
#include "panic.h"
#include "pwm.h"
#include "../customLib/taskScheduler.h"
#include <pacer.h>
#include <math.h>
#include "stdio.h"
#include "delay.h"
#include "mcu_sleep.h"
#include "irq.h"
#include "ledbuffer.h"
#include "ledtape.h"
#include "adc.h"

// Setup pacer, which will be used to control the task scheduler
#define TASK_SCHEDULER_RATE 100 // in HZ


/* Setup the PWM channels */
static const pwm_cfg_t pwm1_cfg =
{
    .pio = MOTOR_LEFT_FORWARD_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0),
    .align = PWM_ALIGN_CENTRE,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm2_cfg =
{
    .pio = MOTOR_RIGHT_FORWARD_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

/* Setup the radio*/
static const spi_cfg_t spi_cfg =
    {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0,
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8
    };

static nrf24_cfg_t nrf24_cfg =
    {
        .channel = RADIO_CHANNEL,
        .address = RADIO_ADDRESS,
        .payload_size = RADIO_PAYLOAD_SIZE,
        .ce_pio = RADIO_CE_PIO,
        // If the IRQ pin is not connected, use a value of 0.
        // .irq_pio = 0,
        .irq_pio = RADIO_IRQ_PIO,
        .spi = spi_cfg,
    };

/* Setup for sleep functionality*/
static mcu_sleep_wakeup_cfg_t muc_sleep_wakeup_cfg = 
{
    .pio = SLEEP_PIO,
    .active_high = false,
};

static mcu_sleep_cfg_t mcu_sleep_cfg = 
{
    .mode = MCU_SLEEP_MODE_BACKUP,
};

nrf24_t *nrf;

/* ADC to read battery voltage*/
static const adc_cfg_t adc_cfg = 
{
    .bits = 12,
    .channels = BIT(BATTERY_ADC_CHANNEL),
    .trigger = ADC_TRIGGER_SW,
    .clock_speed_kHz = 1000
};


int setPWM(int); // Sets motor pwm from radio RX
// int readComm(int); // Reads serial and transmit via radio TX
int radioRx(int); //Reads radi/* Battery*/
int radioTx(int); // Sends radio
int checkBumper(int); // Checks if bumper is pressed
int stopOnBump(int); // counter that runs 2S when pressed
int heartBeat(int);
int update_LEDstrip(int);
int checkBattVoltage(int);

// void channel_A_ISR(void);


// Global variables
int leftDuty = 0;
int rightDuty = 0;
char RADIO_TX_BUFFER[RADIO_PAYLOAD_SIZE + 1];
bool IS_SLEEP = false;
bool IS_BUMPED = false;
bool USB_CONNECTED = false;
bool LEDblue = false;
int LEDcount = 0;
ledbuffer_t *leds;
uint8_t NO_LIGHT[NUM_LEDS * 3];

uint8_t WHITE_STRIP[NUM_LEDS * 3];
uint8_t RED_STRIP[NUM_LEDS * 3];


void channel_A_ISR(void) {
    pio_output_toggle(LED_BLUE_PIO);
    //printf("ISR trigger \n");
    if (pio_input_get(SLEEP_PIO) == 0){
        delay_ms(2);
        if (!pio_input_get(SLEEP_PIO)) {
            if (IS_SLEEP == true){ //WAKE UP CODE
                IS_SLEEP = false;

            } else { // SLEEP CODEPIO_IRQ_FALLING_EDGE
                IS_SLEEP = true;
                delay_ms(500); 
                pio_output_high(LED_RED_PIO);
                pio_output_high(LED_GREEN_PIO);
                pio_output_high(LED_BLUE_PIO);

                ledtape_write (LEDTAPE_PIO, NO_LIGHT, NUM_LEDS * 3);

                setPWM(1);
                // pio_output_high(H_BRIDGE_WAKE_PIO);

                mcu_sleep(&mcu_sleep_cfg);      
            }
        }
    }

    if (!pio_input_get(BUMPER_PIO)){
        delay_ms(2);
        if (!pio_input_get(BUMPER_PIO)) {
            IS_BUMPED = true;
            pio_output_low(LED_RED_PIO);
            ledtape_write (LEDTAPE_PIO, RED_STRIP, NUM_LEDS * 3);
        }
    }

    pio_irq_clear(SLEEP_PIO);
}

#define tasksNum 5
const unsigned char periodPWM = 3; // 100Hz
const unsigned char periodRx = 1; //100Hz
const unsigned char periodTx = 5; // 20Hz
const unsigned char periodBumpDelay = 30; //3Hz
const unsigned char periodBattCheck = 100; // 1hz

task tasks[tasksNum];


int main (void)
{
    unsigned char i = 0; // Note: order tasks in priority of execution
    tasks[i].period = periodBumpDelay; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &stopOnBump;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodRx; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &radioRx;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodBattCheck; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &checkBattVoltage;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodPWM; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &setPWM;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodTx; 
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &radioTx;
    tasks[i].arg = 0; 
    i++;
    // tasks[i].period = periodHeartBeat; 
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].taskHandler = &heartBeat;
    // tasks[i].arg = 0; 
    // i++;

    //Turn Off LEDTAPE state
    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Set full green  GRB order
        NO_LIGHT[j * 3] = 0;
        NO_LIGHT[j * 3 + 1] = 0;
        NO_LIGHT[j * 3 + 2] = 0;
    }

    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Set full green  GRB order
        WHITE_STRIP[j * 3] = LED_BRIGHTNESS;
        WHITE_STRIP[j * 3 + 1] = LED_BRIGHTNESS;
        WHITE_STRIP[j * 3 + 2] = LED_BRIGHTNESS;
    }

    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Set full green  GRB order
        RED_STRIP[j * 3] = 0;
        RED_STRIP[j * 3 + 1] = LED_BRIGHTNESS;
        RED_STRIP[j * 3 + 2] = 0;
    }

    // Check if USB connected
    pio_config_set(USB_DETECT_PIO, PIO_INPUT);
    USB_CONNECTED = pio_input_get(USB_DETECT_PIO);
    // Redirect stdio to usb serial
    if (USB_CONNECTED) {
        usb_serial_stdio_init();
    }
    //LED Tape Config 
    leds = ledbuffer_init (LEDTAPE_PIO, NUM_LEDS);

    // Setup LEDs
    pio_config_set(LED_GREEN_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_BLUE_PIO, PIO_OUTPUT_HIGH);


    #if MOTOR_ENABLE
    pio_config_set(H_BRIDGE_WAKE_PIO, PIO_OUTPUT_HIGH);

    pio_config_set(MOTOR_LEFT_DIR, PIO_OUTPUT_LOW);
    pio_config_set(MOTOR_RIGHT_DIR, PIO_OUTPUT_LOW);
    #endif
    
    nrf24_set_retries(nrf, 1, 1);
    //nrf24_set_auto_ack(nrf, false);

    // Start Pacer
    pacer_init(TASK_SCHEDULER_RATE * 2);

    // Initialise the radio
    pio_config_set(RADIO_CHANNEL_SELECT_1_PIO, PIO_PULLUP);
    pio_config_set(RADIO_CHANNEL_SELECT_2_PIO, PIO_PULLUP);

    // Channel A ISR config
    pio_config_set(SLEEP_PIO, PIO_PULLUP);
    pio_config_set(BUMPER_PIO, PIO_PULLUP);

    pio_irq_config_set(SLEEP_PIO, PIO_IRQ_FALLING_EDGE);

    irq_config(CHANNEL_A_ISQ_ID, 1, channel_A_ISR);
    pio_irq_enable(SLEEP_PIO);
    pio_irq_enable(BUMPER_PIO);

    irq_enable(CHANNEL_A_ISQ_ID);

    bool success = mcu_sleep_wakeup_set(&muc_sleep_wakeup_cfg);
    if (!success) {
        pio_config_set(LED_BLUE_PIO,PIO_OUTPUT_HIGH);

    }


    switch(pio_input_get(RADIO_CHANNEL_SELECT_1_PIO) + (pio_input_get(RADIO_CHANNEL_SELECT_2_PIO) << 1)) {
        case 0: {
            nrf24_cfg.channel = 37;
            break;
        } case 1: {
            nrf24_cfg.channel = 53;
            break;
        } case 2: {
            nrf24_cfg.channel = 82;
            break;
        } case 3: {
            nrf24_cfg.channel = 109;
            break;
        } default: {
            nrf24_cfg.channel = 67;
            break;
        }
    }
    nrf = nrf24_init (&nrf24_cfg); 
    if (! nrf)
        panic (LED_RED_PIO, 2);
    
    delay_ms(10);

    while (1) {

        for (i=0; i < tasksNum; ++i) {
            if (tasks[i].elapsedTime >= tasks[i].period) { // If ready, 
                tasks[i].arg = tasks[i].taskHandler(tasks[i].arg); // Execute the task
                tasks[i].elapsedTime = 0; // reset it's countdown
            }
            // increment the task's counter
            tasks[i].elapsedTime += 1;
        }
        // Once each task has been iterated through, wait for the next pacer tick before starting again
        // pacer_wait();
    }
}

// Heartbeat task
// int heartBeat(int state) {
    
//     count++;
//     printf("Heartbeat - %d\n", count);
//     pio_output_toggle(LED_GREEN_PIO);

//     return 1;
// }

int setPWM(int state) {
    static pwm_t leftPwm;
    static pwm_t rightPwm;

    if (IS_BUMPED || IS_SLEEP) {
        leftDuty = 0;
        rightDuty = 0;
    }
    

    #if MOTOR_ENABLE
    switch (state) {
        case 0: // Initialise the PWM on first call
            leftPwm = pwm_init(&pwm1_cfg);
            rightPwm = pwm_init(&pwm2_cfg);

            if (!leftPwm || !rightPwm) {
                // panic(LED_RED_PIO, 3); // will halt execution
            }
            pwm_channels_start(pwm_channel_mask(leftPwm) | pwm_channel_mask(rightPwm));
            // Don't need  break here, doesn't matter that it falls through
            break;
        case 1: {// Set PWM for both channels with some math,

            if (leftDuty < 0) {
                // pio_output_set(MOTOR_LEFT_DIR, PIO_OUTPUT_LOW);
                pio_output_low(MOTOR_LEFT_DIR);
                pwm_duty_set(leftPwm, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 100 + leftDuty));

            } else {
                // pio_output_set(MOTOR_LEFT_DIR, PIO_OUTPUT_HIGH);
                pio_output_high(MOTOR_LEFT_DIR);
                pwm_duty_set(leftPwm, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, leftDuty));

            }

            if (rightDuty < 0) {
                pio_output_low(MOTOR_RIGHT_DIR);
                pwm_duty_set(rightPwm, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 100 + rightDuty));
            } else {
                pio_output_high(MOTOR_RIGHT_DIR);
                pwm_duty_set(rightPwm, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, rightDuty));
            }

            // Since I can't see the PWM without an oscilloscope, toggle an LED to show something happened
            break;
        }
    }

    #endif
    return 1; // switches the state to 1 after the first function call so that the init only occurs once
}


int radioRx(int state) 
{
    uint8_t bytes;
    int8_t buffer[RADIO_PAYLOAD_SIZE + 1];

    bytes = nrf24_read (nrf, buffer, RADIO_PAYLOAD_SIZE);
    int8_t speed_l, speed_r;
    if (bytes != 0)
    {
        buffer[bytes] = 0;
        speed_l = buffer[0];
        speed_r = buffer[1];
        if(speed_l <= 100 && speed_l >= -100
           && speed_r <=100 && speed_r >= -100) 
        {
            leftDuty = speed_l;
            rightDuty = speed_r;

            // printf("L: %d R: %d\n", leftDuty, rightDuty);
            // pio_output_toggle(LED_BLUE_PIO);

        } else {
            // printf("%s is an invalid input in RX\n", RADIO_RX_BUFFER);
        }        
    } else {
        return 0;
    }
    return 1;
}

int radioTx(int state) 
{

    // snprintf (RADIO_TX_BUFFER, sizeof (RADIO_TX_BUFFER), data);
    int8_t buffer[RADIO_PAYLOAD_SIZE+1];

    // If Bumped, set LSB to 1
    int8_t mask = 1 << BUMP_BIT_POSITION;
    buffer[2] = ((0 & ~mask) | (IS_BUMPED << BUMP_BIT_POSITION)); // for consistency, the first two bytes are reserved for motor speed


    if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE)) {
        // pio_output_set (LED_RED_PIO, PIO_OUTPUT_LOW);
        // printf("Message Failed to send\n");
        return 0;
    } else {
        // pio_output_set (LED_RED_PIO, PIO_OUTPUT_HIGH);
        // printf("Message Sent\n");
        return 1;
    }

    return 1;

}

int stopOnBump(int arg) {
    static int count = TASK_SCHEDULER_RATE * BUMP_DELAY / periodBumpDelay;
    
    if (IS_BUMPED)  {
        count -= 1;
        if (count == 0) {
            IS_BUMPED = false;
            ledtape_write (LEDTAPE_PIO, WHITE_STRIP, NUM_LEDS * 3);
            pio_output_high(LED_RED_PIO);
            count = TASK_SCHEDULER_RATE * BUMP_DELAY / periodBumpDelay;
        }
    }
    return 1;
}

int checkBattVoltage(int state) {
    static adc_t battADC; 
    // pio_output_toggle(LED_BLUE_PIO);
    if (!state) {
        battADC = adc_init(&adc_cfg);
    } else {
        uint16_t data[1];
        adc_read(battADC, data, sizeof(data));
        USB_CONNECTED = pio_input_get(USB_DETECT_PIO);

        if (!USB_CONNECTED && data[0] < BATTERY_LOW_VOLTAGE) {
            panic(LED_RED_PIO, 3);
            IS_SLEEP = true;
            delay_ms(1000);
            mcu_sleep(&mcu_sleep_cfg);
        }
    }
    return 1;
    
}

