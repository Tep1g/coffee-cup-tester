#include "serial.h"
#include "adc.h"
#include "led.h"
#include "lcd.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

static void init(void) {
    T0CS = 0;
    T0CON = 0x87;
    TRISA = 0xFF;
    TRISB = 0;
    TRISC = 0xC0;
    
    SPBRG = 255;
    RCSTA = 0x80;
    TXSTA = 0x20;
    BAUDCON = 0x08;
    
    ADCON2 = 0x91;
    ADCON1 = 0x0C;
    LCD_Init();
    
    GIE = 1;
    PEIE = 1;
    TMR0IP = 1;
    TMR0IE = 1;
    ADON = 1;
    TMR0ON = 1;
    TXIE = 0;
    TXIP = 0;
}

void __interrupt() IntServe(void) {
    if (ADIF) {
        adc_busy = false;
        adc_samples[current_channel] = ADRES;
        current_channel = (current_channel + 1) % 3;
        if (current_channel == 0) {
            //tx_ready = true;
            water_temp_float = get_water_temp();
            water_temp = roundf(water_temp_float);
            update_led(water_temp);
            ambient_temp_float = get_ambient_temp();
            ambient_temp = roundf(ambient_temp_float);
            update_lcd();
            //update_led(water_temp);
            adc_busy = false;
        }
        else {
            adc_start(current_channel);
        }
        ADIF = 0;
    }
    
    if (TXIF && TXIE) {
        if (!msg_sent) {
            while (!TRMT);
            TXREG = msg_arr[msg_index] + 48;
            msg_index++;
            if (msg_index == msg_len) {
                msg_sent = true;
            }
        }
        else {
            while (!TRMT);
            TXREG = newline[newline_index];
            newline_index++;
            if (newline_index == 2) {
                serial_tx_stop();
            }
        }
    }
    
    if (TMR0IF) {
        TMR0 = -31250; //800 ms per interrupt
        adc_start(current_channel);
        TMR0IF = 0;
    }
}

void main(void) {
    init();
    while (1) {
        //temp_var = TMR0;
        if (tx_ready) { //will be interrupted if event ordering saturates
            //msg_len = get_len((uint32_t) water_temp);
            msg_len = get_len((int32_t) water_temp);
            to_msg_array(water_temp);
            serial_tx_start();
            tx_ready = false;
        }
    }
}

