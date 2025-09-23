/** @file   target.h
    @author M. P. Hayes, UCECE
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a racer!  */

/* Testing states */

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

/* USB  */
//#define USB_VBUS_PIO PA5_PIO
#define USB_CURRENT_MA 500
#define USB_DETECT_PIO PA25_PIO

/* LEDs  */
#define LED_RED_PIO PA8_PIO
#define LED_GREEN_PIO PA9_PIO
#define LED_BLUE_PIO PA7_PIO


/* General  */
#define APPENDAGE_PIO PA1_PIO
#define SERVO_PWM_PIO PA2_PIO

/* Button  */
#define BUMPER_PIO PA6_PIO
#define BUMP_BIT_POSITION 0 // LSB of 3rd byte in payload is bumped state
#define BUMPER_IRQ_RD ID_PIOA
#define BUMP_DELAY 7

/* H-bridges   */
#define MOTOR_ENABLE 1

#define H_BRIDGE_WAKE_PIO PA11_PIO

#define MOTOR_LEFT_FORWARD_PWM_PIO PA19_PIO
#define MOTOR_LEFT_DIR PA23_PIO

#define MOTOR_RIGHT_FORWARD_PWM_PIO PA20_PIO
#define MOTOR_RIGHT_DIR PA24_PIO

#define PWM_FREQ_HZ 100 //TODO: test/change this value!

/* PWM test points*/
#define PWM_TEST_POINT_ONE PA25_PIO
#define PWM_TEST_POINT_TWO PA17_PIO

/* Radio  */
#define RADIO_POWER_ENABLE_PIO PA30_PIO

#define RADIO_CS_PIO PA31_PIO
#define RADIO_CE_PIO PB13_PIO
#define RADIO_IRQ_PIO PB14_PIO

#define RADIO_CHANNEL 80
#define RADIO_ADDRESS 0x6969694206LL
#define RADIO_PAYLOAD_SIZE 3

#define RADIO_CHANNEL_SELECT_1_PIO PA1_PIO
#define RADIO_CHANNEL_SELECT_2_PIO PA0_PIO

/* LED tape  */
#define LEDTAPE_PIO PA10_PIO
#define NUM_LEDS 22
#define LED_BRIGHTNESS 128
/* Other PIO */
#define SLEEP_PIO PA2_PIO 
#define CHANNEL_A_ISQ_ID ID_PIOA

/* Battery*/
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_1
#define BATTERY_LOW_VOLTAGE 2762// < 5V




#endif /* TARGET_H  */
