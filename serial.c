/**
 * @file
 * @brief This module contains functions relating to serial transmission.
 */

#include "serial.h"
#include <xc.h>

/**
 * @brief This function gets the number of digits in a number.
 * 
 * @param num A 16 bit integer
 * 
 * @return Return the number of digits in the 16 bit integer.
 */
uint8_t get_len(int32_t num) {
    if (num < 0) num = -num;
    
    uint8_t count = 0;
    do {
        num /= 10;
        count++;
    } 
    while (num != 0);
    return count;
}

/**
 * @brief This function transfers the digits of an 8 bit integer to an array.
 * 
 * @param num An 8 bit integer
 */
void to_msg_array(uint16_t num) {
    for (uint8_t i=msg_len; i>0; i--) {
        msg_arr[i-1] = (num % 10);
        num /= 10;
    }
}

/**
 * @brief This function enables the serial transmit interrupt
 */
void serial_tx_start(void) {
    if (serial_busy) {
        return;
    }
    serial_busy = true;
    TXIE = 1;
    msg_sent = false;
}

/**
 * @brief This function disables the serial transmit interrupt
 */
void serial_tx_stop(void) {
    msg_index = 0;
    msg_len = 0;
    newline_index = 0;
    TXIE = 0;
    serial_busy = false;
}