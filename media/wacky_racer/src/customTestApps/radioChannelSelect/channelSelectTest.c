/*
Author: Matthew King
Brief: Checks the radio channel select inputs are working on both boards
*/

#include "nrf24.h"
#include "target.h"
#include "pio.h"
#include "usb_serial.h"
#include "pacer.h"
#include "spi.h"
#include "delay.h"

/* Setup Radio*/
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
nrf24_t *nrf; // Radio device

int main(void) {
    /* Setup PIO pins*/
    pio_config_set(RADIO_CHANNEL_SELECT_1_PIO, PIO_PULLUP);
    pio_config_set(RADIO_CHANNEL_SELECT_2_PIO, PIO_PULLUP);
    pio_config_set(LED_GREEN_PIO, PIO_OUTPUT_LOW);
    
    usb_serial_stdio_init();

    pacer_init(20*2);

    delay_ms(50);

    while (1) {
        /*Update the radio config with the channel pin*/
        uint8_t channel1 = pio_input_get(RADIO_CHANNEL_SELECT_1_PIO);
        uint8_t channel2 = pio_input_get(RADIO_CHANNEL_SELECT_2_PIO);
        printf("Channel: %d \n", (channel2 << 1) + channel1);
        pacer_wait();
    }
}