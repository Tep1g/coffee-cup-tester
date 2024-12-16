/**
 * @file
 * @brief This module contains functions relating to the LEDs.
 */

#include "led.h"
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#define LED_BLUE RC0
#define LED_GREEN RC1
#define LED_RED RC2

//static bool leds_enabled = false;

/**
 * @brief This function updates the stored temperature value.
 * 
 * @param temp The updated temperature value
 *//*
void update_temp(uint16_t temp) {
    current_temp = temp;
    leds_enabled = true;
    return;
} 
*/
/**
 * @brief This function updates the LEDs using the stored temperature value.
 * 
 * The update_temp function should be called before calling this function.
 */
void update_led(uint16_t temp) {
    if (temp < 140) { //140
        LED_GREEN = 0;
        LED_RED = 0;
        LED_BLUE = 1;
    }
    else if (temp > 175) { //175
        LED_BLUE = 0;
        LED_GREEN = 0;
        LED_RED = 1;
    }
    else {
        LED_BLUE = 0;
        LED_RED = 0;
        LED_GREEN = 1;
    }
    return;
}