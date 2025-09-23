/* File:   radio_tx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
*/
#include "nrf24.h"
#include "pio.h"
#include "pacer.h"
#include "stdio.h"
#include "delay.h"
#include "panic.h"
#include "usb_serial.h"
#include "target.h"

#define RADIO_CHANNEL 5
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 2

int main (void)
{
    spi_cfg_t spi_cfg =
        {
            .channel = 0,
            .clock_speed_kHz = 1000,
            .cs = RADIO_CS_PIO,
            .mode = SPI_MODE_0,
            .cs_mode = SPI_CS_MODE_FRAME,
            .bits = 8
        };
    nrf24_cfg_t nrf24_cfg =
        {
            .channel = RADIO_CHANNEL,
            .address = RADIO_ADDRESS,
            .payload_size = RADIO_PAYLOAD_SIZE,
            .ce_pio = RADIO_CE_PIO,
            .irq_pio = RADIO_IRQ_PIO,
            .spi = spi_cfg,
        };
    uint8_t count = 0;
    nrf24_t *nrf;

    // Configure LED PIO as output.
    pio_config_set (LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_GREEN_PIO, PIO_OUTPUT_LOW);
    pio_config_set (LED_BLUE_PIO, PIO_OUTPUT_HIGH);
    pacer_init (10);

    usb_serial_stdio_init();

    #ifdef RADIO_POWER_ENABLE_PIO
        // Enable radio regulator if present.
        pio_config_set (RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
        delay_ms (10);
    #endif

    nrf = nrf24_init (&nrf24_cfg);
    if (! nrf)
        panic (LED_RED_PIO, 2);
    
    delay_ms(10);

    while (1)
    {
        int8_t buffer[RADIO_PAYLOAD_SIZE + 1];
        int8_t speed1 = 50;
        int8_t speed2 = 75;
        pacer_wait ();
        // pio_output_toggle (LED_BLUE_PIO);

        // snprintf (buffer, sizeof (buffer), (speed1 << 8) + speed2, count++);
        // buffer[0] = speed1;
        // buffer[1] = speed2;

        buffer[0] = speed1;
        buffer[1] = speed2;

        if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE)) {
            pio_output_low(LED_RED_PIO);
            printf("Message Failed\n");
        } else {
            pio_output_high(LED_RED_PIO);
            // printf("Message Sent: %d\n", buffer);
            for (uint8_t i = 0; i < RADIO_PAYLOAD_SIZE; i++) {
                printf("Byte %d: %d\n", i, buffer[i]);
            }
        }
    }
}
