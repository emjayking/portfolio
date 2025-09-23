/*
Author: Matthew King
Brief: Test the functionality of the mmelody library
*/

#include "target.h"
#include "mmelody.h"
#include "pacer.h"
#include "usb_serial.h"
#include "delay.h"
#include "pwm.h"
#include "pio.h"
#include <math.h>


// Define the Happy Birthday melody as a string
const char test_song[] = "F4, F4, E4, D4, C4, C4, D4, E4, F4, F4, E4, D4, C4, C4, D4, E4, F4, F4, E4, D4, C4, C4, A4, A4, A4, A4, A4, A4, A4, A4, -, -, F4, F4, E4, D4, C4, C4, D4, E4, F4, F4, E4, D4, C4, C4, D4, E4,  E4, F4, F4, E4, D4, C4, C4, A4, A4, A4, A4, A4, A4, D4, D4, D4, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, D4, D4, -, -, G#4, G#4, -, -, D5, D5, G#4, G#4, -, -, -, -, G#4, G#4, -, -, D5, D5, G#4, G#4, -, -, D4, D4, D4, D4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, D4, D4, -, -, G#4, G#4, -, -, D5, D5, G#4, G#4, -, -, -, -, G#4, G#4, -, -, D5, D5, G#4, G#4, -, -, D4, D4, D4, D4, -, -, -, -, C4, C4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, G4, G4, A4, A4, -, -, G4, G4, G4, G4, -, -, -, -, F4, F4, -, -, D4, D4, F4, F4, F4, F4, D4, D4, D4, D4, A4, A4, A4, A4, A#4, A#4, A4, A4, -, -, G4, G4, G4, G4, -, -, F4, F4, F4, C4, D4, D4, D4, -, F4, F4, F4, C4, D4, D4, D4, -, F4, F4, F4, C4, D4, D4, D4, -, F4, F4, F4, C4, D4, D4, D4";
pwm_t pwm_channel;  // Global variable for the PWM channel


double midi_to_frequency(uint8_t midi_note)
{
    return 440.0 * pow(2.0, (midi_note - 69) / 12.0);
}

void play_note_callback(void *data, uint8_t note, uint8_t volume)
{
    //print("Playing Note: %u\n", note)
    if (note != 0) // Ensure the note is not zero
        {
            double frequency = midi_to_frequency(note);
            printf("Playing note: %u at frequency: %.2f Hz at volume: %u\n", note, frequency, volume);
            
            // Set the PWM frequency and duty cycle
            pwm_frequency_set(pwm_channel, (pwm_frequency_t)frequency);
            pwm_duty_ppt_set(pwm_channel, 500);  // Set duty cycle to 50%
            
            pwm_start(pwm_channel);
        }
    else
        {
            printf("Rest or end of melody\n");
            pwm_stop(pwm_channel);
        }
}



int main(void)
{
    pacer_init (1000);
    usb_serial_stdio_init ();
    // Define and initialize melody object
    mmelody_obj_t melody_obj;
    mmelody_t melody = mmelody_init(&melody_obj, 100, play_note_callback, NULL);
    
    // Set the melody speed and volume
    mmelody_speed_set(melody, MMELODY_SPEED_DEFAULT);
    mmelody_volume_set(melody, 100);
    
    // Play the Happy Birthday melody
    mmelody_play(melody, test_song);


    pwm_cfg_t pwm_cfg = {
        .pio = BUZZER_PWM_PIO,  // Set to the desired pin
        .frequency = 440,  // Initial frequency, will be updated in callback
        .duty_ppt = 500,   // 50% duty cycle
        .align = PWM_ALIGN_LEFT,
        .polarity = PWM_POLARITY_HIGH,
        .stop_state = PIO_OUTPUT_LOW
    };
    pwm_channel = pwm_init(&pwm_cfg);
    
    // Main loop to update the melody
    while (1)
    {
        mmelody_update(melody);

        // if (melody_obj.cur == NULL || *melody_obj.cur == '\0')
        // {
        //     printf("Melody finished, restarting...\n");
        //     mmelody_play(melody, happy_birthday_melody);
        // }

        pacer_wait();
        //delay_ms(300);
    }
    
    return 0;
}
