/**
 * @file
 * @brief This module contains functions relating to the ADC.
 */

#include "adc.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define MAX_CHANNEL 2
#define MIN_CHANNEL 0
#define NUM_WATER_CHANNELS 2

/**
 * @brief This functions starts an adc acquisition.
 * 
 * @param channel an adc channel
 * 
 * This function enables the adc interrupt.
 */
void adc_start(uint8_t channel) {
    if ((channel > MAX_CHANNEL) || (channel < MIN_CHANNEL)) 
        return;
    if (adc_busy) return;
    ADCON0 = ((channels[channel] << 2) | 0x01);
    adc_busy = true;
    ADIE = 1;
    GODONE = 1;
}

float get_water_temp(void) {
    float adc_average = 0.0f;
    for (uint8_t i=0; i < NUM_WATER_CHANNELS; i++) {
        adc_average += adc_samples[i];
    }
    adc_average = adc_average/NUM_WATER_CHANNELS;
    float v = adc_average*5/1023.0f;
    float T = (3969/ (logf((v/(5-v))) + (3969/298.0f))) - 273;
    float T_f = ((9*T)/5) + 32;
    return T_f;
}

float get_ambient_temp(void) {
    float v = adc_samples[2]*5/1023.0f;
    float T = (3892/ (logf((v/(5-v))) + (3892/298.0f))) - 273;
    float T_f = ((9*T)/5) + 32;
    return T_f;
}