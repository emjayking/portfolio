/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a hat!  */

/* System clocks  */
#define F_XTAL 12.00e6
#define MCU_PLL_MUL 16
#define MCU_PLL_DIV 1

#define MCU_USB_DIV 2

/* 192 MHz  */
#define F_PLL (F_XTAL / MCU_PLL_DIV * MCU_PLL_MUL)

/* 96 MHz  */
#define F_CPU (F_PLL / 2)

/* TWI  */
#define TWI_TIMEOUT_US_DEFAULT 10000
#define MPU_ADDRESS 0x68

/* USB  */
//#define USB_VBUS_PIO PA5_PIO
#define USB_CURRENT_MA 500
// #define USB_DETECT_ADC ADC_CHANNEL_5
// #define USB_DETECT_THRESHOLD 4095
#define USB_DETECT_PIO PB1_PIO

/* ADC  */
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_3
#define BATTERY_LOW_VOLTAGE 2680
/* Accelerometer  */
#define ADXL345_ADDRESS 0x53
#define ADXL345_INTERRUPT_PIO PA10_PIO

/* LEDs  */
#define LED_BLUE_PIO PA9_PIO // blue
#define LED_RED_PIO PA8_PIO // red
#define LED_GREEN_PIO PA7_PIO // green & PWM

/* dip switch  */
#define SWITCH_CHANNEL_1_PIO PA28_PIO
#define SWITCH_CHANNEL_2_PIO PA27_PIO


/* PWM (Only used for testing in the absence of serial comms)*/
#define PWM_TESTPOINT_PIO PA15_PIO // only have a single PWM test point
#define PWM_FREQ_HZ 100 //TODO: test/change this value!

/* Button  */
#define SLEEP_BUTTON_PIO PA2_PIO

/* Radio  */
#define RADIO_CS_PIO PA6_PIO
#define RADIO_CE_PIO PA5_PIO
#define RADIO_IRQ_PIO PA26_PIO

#define RADIO_POWER_ENABLE_PIO PA30_PIO 

#define RADIO_CHANNEL_SELECT_1_PIO PA1_PIO
#define RADIO_CHANNEL_SELECT_2_PIO PA0_PIO

#define RADIO_CHANNEL 80
#define RADIO_ADDRESS 0x6969694206LL
#define RADIO_PAYLOAD_SIZE 3

#define BUMPED_BIT_MASK 1 << 0 // First bit of 3rd byte determines if bumped

/* LED tape  */
#define LEDTAPE_PIO PA10_PIO
#define LED_ACTIVE 0
#define NUM_LEDS 22
#define LED_BRIGHTNESS 128
/* Potentiometers */
#define GAIN_POTENTIOMETER_ADC ADC_CHANNEL_8
#define TRIM_POTENTIOMETER_ADC ADC_CHANNEL_1
#define DEADBAND_POTENTIOMETER_ADC ADC_CHANNEL_0

/* Buzzer */
#define BUZZER_PWM_PIO PA23_PIO
#define BUZZER_FREQUENCY_HZ 150 // nice low freq
#define MIDI_NOTE_MIN 21
#define MIDI_NOTE_MAX 108
#define HOLY_GRAIL_SPEED 220
#define INTERMISSION_SPEED 110

#define PLAY_MUSIC 1

/* Sleep */
#define SLEEP_PIO PA2_PIO
#define SLEEP_BUTTON_IRQ_RD ID_PIOA

#define GOLD_R LED_BRIGHTNESS
#define GOLD_G 88
#define GOLD_B 0

#define JEWEL_WHITE_R LED_BRIGHTNESS
#define JEWEL_WHITE_G LED_BRIGHTNESS
#define JEWEL_WHITE_B LED_BRIGHTNESS

#define JEWEL_GREEN_R 0
#define JEWEL_GREEN_G LED_BRIGHTNESS
#define JEWEL_GREEN_B 0

#define JEWEL_RED_R LED_BRIGHTNESS
#define JEWEL_RED_G 0
#define JEWEL_RED_B 0

#endif /* TARGET_H  */