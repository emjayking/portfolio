/* File:   button_test1.c
   Author: M. P. Hayes, UCECE
   Date:   18 Dec 2021
   Descr:  Simple button test demo without debouncing
*/
#include "mcu.h"
#include "pio.h"
#include "pacer.h"

#define PACER_RATE 100

int
main (void)
{
    /* Configure LED PIO as output.  */
    pio_config_set (LED_GREEN_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_RED_PIO, PIO_OUTPUT_LOW);
    /* Configure button PIO as input with pullup.  */
    pio_config_set (USB_DETECT_PIO, PIO_INPUT);

    pacer_init (PACER_RATE);

    while (1)
    {
        /* Wait until next clock tick.  */
        pacer_wait ();

        if (pio_input_get (USB_DETECT_PIO))
            pio_output_low(LED_GREEN_PIO);
        else
            pio_output_high (LED_GREEN_PIO);
    }
}
