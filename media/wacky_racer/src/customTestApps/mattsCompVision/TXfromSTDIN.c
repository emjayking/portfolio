/*
 @author: Matthew King
 @brief: Recieves input from stdin and transmits over radio to the car
*/

#include "nrf24.h"
#include "pio.h"
#include "pacer.h"
#include "usb_serial.h"
#include "panic.h"
#include "stdio.h"

// Setup radio

static const spi_cfg_t spi_cfg = {
    .channel = 0,
    .clock_speed_kHz = 1000,
    .cs = RADIO_CS_PIO,
    .mode = SPI_MODE_0,
    .cs_mode = SPI_CS_MODE_FRAME,
    .bits = 8
};

static nrf24_cfg_t nrf24_cfg = {
    .channel = RADIO_CHANNEL,
    .address = RADIO_ADDRESS,
    .payload_size = RADIO_PAYLOAD_SIZE,
    .ce_pio = RADIO_CE_PIO,
    .irq_pio = RADIO_IRQ_PIO,
    .spi = spi_cfg,
};

nrf24_t *nrf;

int main(void) {

    // Power indication
    pio_config_set(LED_GREEN_PIO, PIO_OUTPUT_LOW);
    pio_config_set(LED_RED_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_BLUE_PIO, PIO_OUTPUT_HIGH);

    // Start reading serial
    usb_serial_stdio_init();

    // Setup radio
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    // Initialise the radio channel select
    pio_config_set(RADIO_CHANNEL_SELECT_1_PIO, PIO_PULLUP);
    pio_config_set(RADIO_CHANNEL_SELECT_2_PIO, PIO_PULLUP);
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

    // Start Pacer
    pacer_init(2 * 100);

    // Main loop
    while (1) {
        // Read serial
        char buffer[50];
        char *str;

        str = fgets(buffer, sizeof(buffer), stdin);
        /// Flush the buffer

        if (str) {
            
            pio_output_toggle(LED_BLUE_PIO);
            printf(str);
            // while ((getchar()) != '\n');
            int8_t speed_l, speed_r;

            if (sscanf(str, "%d %d",&speed_l, &speed_r) 
            && speed_l <= 100 && speed_l >= -100
            && speed_r <= 100 && speed_r >= -100) {
                
                // Transmit via radio
                int8_t txBuffer[3];
                txBuffer[0] = speed_l;
                txBuffer[1] = speed_r;

                if (!nrf24_write(nrf, txBuffer, RADIO_PAYLOAD_SIZE)) {
                    pio_output_low(LED_RED_PIO);
                } else {
                    pio_output_high(LED_RED_PIO);
                }
            }

        }
        pacer_wait();        
    }
}
