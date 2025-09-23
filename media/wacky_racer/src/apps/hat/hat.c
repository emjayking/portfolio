/**
 @file hat.c
 @author Matthew King
 @date 25/04/2024
 @brief main operating system for hat PCB
*/

#include "target.h"
#include "pio.h"
#include "adxl345.h"
#include "usb_serial.h"
#include "panic.h"
#include "pwm.h"
#include "adc.h"
#include "../customLib/taskScheduler.h"
#include <pacer.h>
#include <math.h>
#include "spi.h"
#include "nrf24.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "ledbuffer.h"
#include "ledtape.h"
#include "mcu_sleep.h"
#include "irq.h"
#include "mmelody.h"
#include "../customLib/music.h"

// Setup pacer, which will be used to control the task scheduler
#define TASK_SCHEDULER_RATE 100 // in HZ

// Define the Pi constant if not already included by compiler
#ifndef M_PI
#define M_PI acos(-1)
#endif

/* Setup the accelerometer */
static const twi_cfg_t adxl345_twi_cfg =
{
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR (100000), // 100 kHz
    .slave_addr = 0
};

/* Setup the ADC channels */
static const adc_cfg_t adc_cfg = 
{
    .bits = 12,
    .channels = BIT(TRIM_POTENTIOMETER_ADC) | BIT(GAIN_POTENTIOMETER_ADC) | BIT(DEADBAND_POTENTIOMETER_ADC) | BIT(BATTERY_ADC_CHANNEL),
    .trigger = ADC_TRIGGER_SW,
    .clock_speed_kHz = 1000
};

/* Setup SPI*/
static const spi_cfg_t spi_cfg =
{
    .channel = 0,
    .clock_speed_kHz = 1000,
    .cs = RADIO_CS_PIO,
    .mode = SPI_MODE_0,
    .cs_mode = SPI_CS_MODE_FRAME,
    .bits = 8
};

/* Configure radio */
static nrf24_cfg_t nrf24_cfg =
{
    .channel = RADIO_CHANNEL,
    .address = RADIO_ADDRESS,
    .payload_size = RADIO_PAYLOAD_SIZE,
    .ce_pio = RADIO_CE_PIO,
    .irq_pio = RADIO_IRQ_PIO,
    .spi = spi_cfg,
};
nrf24_t *nrf;

/* PWM for the buzzer*/
static const pwm_cfg_t buzz_cfg =
{
    .pio = BUZZER_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (440),
    .duty = PWM_DUTY_DIVISOR (BUZZER_FREQUENCY_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

pwm_t buzzer_pwm;

static mcu_sleep_wakeup_cfg_t muc_sleep_wakeup_cfg = 
{
    .pio = SLEEP_PIO,
    .active_high = false,
};

static mcu_sleep_cfg_t mcu_sleep_cfg = 
{
    .mode = MCU_SLEEP_MODE_BACKUP,
};


/* Declare the handler functions used in the task scheduler */
int statusLedToggle(int); // Toggle Green LED for power indicator
int pollAccelerometer(int); // Reads accelerometer and stores in accel vector
int calculateOrientation(int); // Calculates orientation from accel and stores in angularPos vector
int readADC(int);  // Reads gain, trim, deadband and vbat from the adc
int calculateSpeed(int); // Calculates L/R speed from angularPos and stores in speed vector
int transmit_packet(int);     // Transmits a packet of data
int transmit_data(int);
int recieve_data(int);
int update_LEDstrip(int);
int heartBeat(int);
int update_song(int);
// int transmitRadio(int); // Transmits speed over radio, recieves a notification when bumped


/* Global variables used by scheduled functions */
int16_t accel[3]; // Acelerometer values
int16_t angularPos[2]; // Roll and Tilt from accelerometer
int16_t speeds[2]; // Left and right motor speeds
int8_t dastardlyStuff;

int16_t gain;
int16_t trim;
int16_t deadband;

mmelody_obj_t melody_obj; // Global melody object
mmelody_t melody;         // Global melody pointer

bool LEDblue = false;
int LEDcount = 0;
uint8_t NO_LIGHT[NUM_LEDS * 3];
ledbuffer_t *leds;

bool IS_SLEEP = false;
bool IS_BUMPED = false;
bool IS_BUMPED_PREV = false;
bool USB_CONNECTED = false;

int16_t map(int16_t val, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) 
{
    /* Usefull function from Arduino, maps values from one range to another*/
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Setup the task scheduler
#define tasksNum 8
task tasks[tasksNum];
const unsigned char periodAccel = 2; // Poll 10 times per second
const unsigned char periodOrient = 2; // Calculate an orientation 5 times per second
const unsigned char periodADC = 100; // Run 4 times a second
const unsigned char periodSpeed = 2; // Calculate pwm values from orientatin
const unsigned char periodTX = 1;
const unsigned char periodRX = 5;
const unsigned char preiodLEDstrip = 10;
// const unsigned char preiodHeartBeat = 50;
const unsigned char periodSong = 8;


void sleep_ISR(void) {
    if (pio_input_get(SLEEP_PIO) == 0){
        if (IS_SLEEP){ //WAKE UP CODE
        IS_SLEEP = false;
        
        //Turn off radio
        pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);

        //Turn on SPI
        nrf = nrf24_init (&nrf24_cfg);
        if (! nrf) {
            panic (LED_RED_PIO, 2);
        }

        //Turn on PWM
        pwm_start(buzzer_pwm);
        

        } else { // SLEEP CODEPIO_IRQ_FALLING_EDGE
            IS_SLEEP = true;
            delay_ms(250); 

            //Turn Off Radio
            pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_LOW);

            //Turn off lights
            pio_output_high(LED_BLUE_PIO);
            pio_output_high(LED_GREEN_PIO);
            pio_output_high(LED_RED_PIO);

            //turn off buzzer channel 
            pwm_shutdown();

            //turn off led tape
            ledtape_write (LEDTAPE_PIO, NO_LIGHT, NUM_LEDS * 3);

            mcu_sleep(&mcu_sleep_cfg);   
        }
    }
    pio_irq_clear(SLEEP_PIO);
}

void play_note_callback(void *data, uint8_t note, uint8_t volume)
{
    //printf("running note callback\n");
    if (note >= MIDI_NOTE_MIN && note <= MIDI_NOTE_MAX)
        {
            float frequency = midi_frequencies[note - MIDI_NOTE_MIN];
            //printf("Playing note: %u at frequency: %.2f Hz at volume: %u\n", note, frequency, volume);
            
            // Set the PWM frequency and duty cycle
            pwm_frequency_set(buzzer_pwm, (pwm_frequency_t)frequency);
            // Have to update duty cycle since frequency has changed
            pwm_duty_set(buzzer_pwm, PWM_DUTY_DIVISOR (frequency, 50));
            if(PLAY_MUSIC) {
                pwm_start(buzzer_pwm);
            }
        }
    else
        {
            //printf("Rest or end of melody\n");
            pwm_stop(buzzer_pwm);
        }
}

int main (void)
{
    unsigned char i = 0;
    tasks[i].period = periodTX;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &transmit_data;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodADC;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &readADC;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodRX;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &recieve_data;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodAccel;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &pollAccelerometer;
    tasks[i].arg = 0; // set to initialise
    i++;
    tasks[i].period = periodOrient;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &calculateOrientation;
    tasks[i].arg = 0; // unused
    i++;
    tasks[i].period = periodSpeed;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &calculateSpeed;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = periodSong;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &update_song;
    tasks[i].arg = 0; 
    i++;
    tasks[i].period = preiodLEDstrip;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskHandler = &update_LEDstrip;
    tasks[i].arg = 0; 
    i++;
    // tasks[i].period = preiodHeartBeat;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].taskHandler = &heartBeat;
    // tasks[i].arg = 0; 
    // i++;


    // Setup LED strip
    leds = ledbuffer_init (LEDTAPE_PIO, NUM_LEDS);


    // Setup LEDs (active low)
    pio_config_set(LED_GREEN_PIO, PIO_OUTPUT_LOW);
    pio_config_set(LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_BLUE_PIO, PIO_OUTPUT_HIGH);

    // Setup USB detection
    pio_config_set(USB_DETECT_PIO, PIO_INPUT);
    // Redirect stdio to usb serial
    if (pio_input_get(USB_DETECT_PIO)) {
        
        USB_CONNECTED = true;
    }
    usb_serial_stdio_init();

    // Start the pacer
    pacer_init(TASK_SCHEDULER_RATE * 2);

    // Send power to radio
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    delay_ms(10);

    // Initialise the radio
    pio_config_set(RADIO_CHANNEL_SELECT_1_PIO, PIO_PULLUP);
    pio_config_set(RADIO_CHANNEL_SELECT_2_PIO, PIO_PULLUP);

    // // Sleep IRQ Pin config
    pio_config_set(SLEEP_PIO, PIO_PULLUP);
    pio_irq_config_set(SLEEP_PIO, PIO_IRQ_FALLING_EDGE);
    irq_config(SLEEP_BUTTON_IRQ_RD, 5, sleep_ISR);
    pio_irq_enable(SLEEP_PIO);
    irq_enable(SLEEP_BUTTON_IRQ_RD);

    

    if (!mcu_sleep_wakeup_set(&muc_sleep_wakeup_cfg)) {
        pio_output_low(LED_RED_PIO);
    }


    //Turn Off LEDTAPE state
    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Set full green  GRB order
        NO_LIGHT[j * 3] = 0;
        NO_LIGHT[j * 3 + 1] = 0;
        NO_LIGHT[j * 3 + 2] = 0;
    }


    // Update the radio channel
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
    if (! nrf) {
        panic (LED_RED_PIO, 2);
    }

    //COMMENT OUT FOR RACE
    //
    //
    //
    nrf24_set_retries(nrf, 1, 1);
    //nrf24_set_auto_ack(nrf, false);
    // 
    //
    //
    //COMMENT OUT FOR RACE
    
    // Start the buzzer & initalise melody
    buzzer_pwm = pwm_init(&buzz_cfg);
    melody = mmelody_init(&melody_obj, 3, play_note_callback, NULL);
    
    mmelody_speed_set(melody, 120); 
    mmelody_volume_set(melody, 100);
    
    
    mmelody_play(melody, holy_grail_theme);

    // Main loop
    while (1){

        for (i=0; i < tasksNum; ++i) {
            if (tasks[i].elapsedTime >= tasks[i].period) { // If ready, 
                tasks[i].arg = tasks[i].taskHandler(tasks[i].arg); // Execute the task
                tasks[i].elapsedTime = 0; // reset it's countdown
            }
            // increment the task's counter
            tasks[i].elapsedTime += 1;
        }
        // pacer_wait();

    };
}

// int heartBeat(int state) {
//     pio_output_toggle(LED_BLUE_PIO);
//     // printf("Hello\n");

//     return 1;
// }

int pollAccelerometer(int state) {
    static twi_t adxl345_twi;  
    static adxl345_t *adxl345;
    // Implements a FSM so first function call initialises the ADXL
    if (!state) {
        // Initialise the adxl
        // Setup the I2C
        adxl345_twi = twi_init(&adxl345_twi_cfg);
        // Panic if it fails
        if (!adxl345_twi) {
            panic (LED_RED_PIO, 1);
        }
        // Setup the adxl
        adxl345 = adxl345_init(adxl345_twi, ADXL345_ADDRESS);
        // Panic if it fails
        if (!adxl345) {
            panic(LED_RED_PIO, 2);
        }
    } else  {
        if (adxl345_is_ready(adxl345)) {   // Check if the adxl is ready
            if (adxl345_accel_read(adxl345, accel)) { // Sample the adxl
                //printf ("x: %5d  y: %5d  z: %5d\n", accel[0], accel[1], accel[2]);
            } else {
                printf ("ERROR: failed to read acceleration\n");
            }
        } else {
            printf ("ERROR: Accelerometer not ready\n");
        }
    } 
    return 1;      
}

int calculateOrientation (int arg) {
    /* Update roll and pitch with acceleration values */
    angularPos[0] = atan(accel[1] / sqrt(pow(accel[0], 2) + pow(accel[2], 2))) * 180 / M_PI; // Roll
    angularPos[1] = atan(-1*accel[0] / sqrt(pow(accel[1],2) + pow(accel[2],2))) * 180 / M_PI; // Pitch

    //TODO: implement a  lowpass filter of some kind?

    // printf ("pitch: %5d  roll: %5d \n", angularPos[0], angularPos[1]);

    return 1;
}

int readADC(int state) {
    //printf("running ADC \n");
    /* Reads values from 3 potentiometers and the battery Voltage */
    static adc_t adc;
    if (!state) {
        adc = adc_init(&adc_cfg);
        if (!adc) {
            panic(LED_RED_PIO, 3);
        }
    } else {
        uint16_t data[4]; 

        // Note: data is stored in order of adc channel
        /* Data = [Deadband, Trim, vBat, Gain]*/
        adc_read(adc, data, sizeof(data));
        
        uint16_t newDeadband = map(data[0], 0, 4096, 0, 100);
        if (newDeadband != deadband) {
            printf("Deadband Set to: %3d \n", newDeadband);
            deadband = newDeadband;
        }
        uint16_t newTrim = map(data[1], 0, 4096, 0, 90); // max angle is 90 deg
        if (newTrim != trim) {
            printf("Trim Set to: %3d \n", newTrim);
            trim = newTrim;
        }            
        uint16_t newGain = map(data[3], 0, 4096, 0, 100);
        if (newGain != gain) {
            printf("Gain Set to: %3d \n", newGain);
            gain = newGain;
        }
        // Update usb_connected just to be sure
        USB_CONNECTED = pio_input_get(USB_DETECT_PIO);
        // Check we are above the voltage limit and usb is not detected
        if (data[2] < BATTERY_LOW_VOLTAGE && !USB_CONNECTED) {
            //panic(LED_RED_PIO, 3);
            // Put the MCU to sleep
            //IS_SLEEP = true;
            //delay_ms(1000);
            //mcu_sleep(&mcu_sleep_cfg);
        }
    }
    return 1;
}

int abs(int val){
    if (val > 0) {
        return val;
    } else {
        return - val;
    }
}

int calculateSpeed(int arg) {
    /* Calculate L/R Speeds with gain, trim, yaw and angular pos*/
    
    int16_t speedVecs[2]; // Forward speed and turning speed

    // Apply values from potentiometer
    for (unsigned char i =0; i < 2; i++) {
        if (angularPos[i] > trim) { // Think of trim like max angle
            speedVecs[i] = trim;
            // printf("Limiting %d to %d", i, speedVecs[i]);
        } else if (abs(angularPos[i]) < deadband) { // Deadband acts like min angle
            speedVecs[i] = 0;
            // printf("Flooring %d to 0", i);
        } else if (angularPos[i] < -trim) {
            speedVecs[i] = -trim;
            // printf("Limiting %d to %d", i, speedVecs[i]);
        } else {
            speedVecs[i] = angularPos[i];
            // printf("Setting %d to %d", i, angularPos[i]);
        }
        speedVecs[i] = map(speedVecs[i], -trim, trim, -gain, gain); // Gain acts as max speed
    }
    speedVecs[1] = speedVecs[1] / 2; // Roll is half as sensitive as pitch
    speeds[0] = speedVecs[0] - speedVecs[1]; // Left speed = pitch - roll
    speeds[1] = speedVecs[0] + speedVecs[1]; // Right = pitch + roll

    // Cap values at +- gain once again
    for (unsigned char i =0; i<2; i++) {
        if (speeds[i] > gain) {
            speeds[i] = gain;
            // printf("Speed %d is above gain", i);

        } else if (speeds[i] < (-gain)) {
            // printf("Speed %d is below gain", i);
            speeds[i] = -gain; 
        }
    }

    // Print results
    //printf("L: %d, R: %d \n", speeds[0], speeds[1]);

    return 1;
} 

int transmit_data(int arg) {
    int8_t buffer[RADIO_PAYLOAD_SIZE + 1];

    //pacer_wait ();
    // pio_output_toggle (LED_RED_PIO);

    buffer[0] = (int8_t)speeds[0];
    buffer[1] = (int8_t)speeds[1];

    if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE)) {
        // pio_output_set (LED_RED_PIO, 0);
        // printf("Message Failed\n");
    } else {
        // pio_output_set (LED_RED_PIO, 1);
        // printf("Message Sent\n");
    }
    return 1;
}


int recieve_data(int arg) {
    
    int8_t buffer[RADIO_PAYLOAD_SIZE + 1];
    uint8_t bytes;

    bytes = nrf24_read (nrf, buffer, RADIO_PAYLOAD_SIZE);
    if (bytes != 0)
    {
        buffer[bytes] = 0; 
        IS_BUMPED = buffer[2] & BUMPED_BIT_MASK;

    
    } else {
        // printf("Message Not Recieved\n");
        // pio_output_low(LED_RED_PIO); // Nothing is recieving
    }
    return 1;
}

int update_LEDstrip(int arg){
    // Set the entire strip to gold
    for (int i = 0; i < NUM_LEDS; ++i) {
        ledbuffer_set(leds, i, GOLD_R, GOLD_G, GOLD_B);
    }

    // Add moving jewels
    if (LEDcount % 3 == 0) {
        ledbuffer_set(leds, LEDcount % NUM_LEDS, JEWEL_WHITE_R, JEWEL_WHITE_G, JEWEL_WHITE_B);
    } else if (LEDcount % 3 == 1) {
        ledbuffer_set(leds, LEDcount % NUM_LEDS, JEWEL_GREEN_R, JEWEL_GREEN_G, JEWEL_GREEN_B);
    } else if (LEDcount % 3 == 2) {
        ledbuffer_set(leds, LEDcount % NUM_LEDS, JEWEL_RED_R, JEWEL_RED_G, JEWEL_RED_B);
    }

    LEDcount++;
    ledbuffer_write(leds);
    ledbuffer_advance(leds, 1);

    return 1;
}

int update_song(int arg){
    pio_output_toggle(LED_BLUE_PIO);

    if (IS_BUMPED != IS_BUMPED_PREV ) {
        mmelody_play(melody, intermission_song);  
        mmelody_speed_set(melody, 120);  
    } if (IS_BUMPED == false && IS_BUMPED_PREV) {
        mmelody_play(melody, holy_grail_theme);  
        mmelody_speed_set(melody, 120);  
    }
    IS_BUMPED_PREV = IS_BUMPED;

    mmelody_update(melody);
    if (*melody_obj.cur == '\0')
    {
        mmelody_play(melody, holy_grail_theme);
    }

   
    return 1 ;
}
  
