/* File:   radio_rx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
*/
#include "nrf24.h"
#include "usb_serial.h"
#include "spi.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"


#define RADIO_CHANNEL 5
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 2

int main(void)
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
            // If the IRQ pin is not connected, use a value of 0.
            // .irq_pio = 0,
            .irq_pio = RADIO_IRQ_PIO,
            .spi = spi_cfg,
        };
    nrf24_t *nrf;

    // Configure LED PIO as output and turn them off.
    pio_config_set (LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_BLUE_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_GREEN_PIO, PIO_OUTPUT_LOW); // You've got power!
    delay_ms(50);
    // Redirect stdio to USB serial.
    usb_serial_stdio_init ();

    // 
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);

    // Enable radio regulator if present.
    delay_ms(500);

    nrf = nrf24_init (&nrf24_cfg);
    
    if (! nrf) {
        panic (LED_RED_PIO, 2);
    }
    delay_ms(5000);

    while(1)
    {
        int8_t buffer[RADIO_PAYLOAD_SIZE + 1];
        uint8_t bytes;

        bytes = nrf24_read (nrf, buffer, RADIO_PAYLOAD_SIZE);
        if (bytes != 0)
        {
            // buffer[bytes] = 0;
            printf("Bytes %d\n", bytes);
            // printf ("%d\n", buffer);
            // printf("First Item %d \n", (buffer[0]));
            // printf("Second Item %d \n", (buffer[1]));
            // printf("Third Item %d \n", (buffer[2]));
            for (uint8_t i=0; i<bytes; i++) {
                printf("Byte %d: %d\n", i, buffer[i]);
            }

            // pio_output_toggle (LED_RED_PIO);
            pio_output_high(LED_RED_PIO);
        } else {
            pio_output_low(LED_RED_PIO);
            // printf("Message Failed\n");
        }

    }
}
