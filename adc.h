#ifndef ADC_H
#define	ADC_H

#include <stdint.h>
#include <stdbool.h>

#define CHANNEL_0 0
#define CHANNEL_1 1
#define CHANNEL_2 2
#define NUM_CHANNELS 3

uint8_t channels[NUM_CHANNELS] = {CHANNEL_0, CHANNEL_1, CHANNEL_2};
bool adc_busy = false;
uint8_t current_channel = 0;
uint16_t adc_samples[NUM_CHANNELS] = {0};
bool adc_samples_ready = false;

void adc_start(uint8_t channel);

float get_water_temp(void);

float get_ambient_temp(void);

#endif	/* ADC_H */