#ifndef SERIAL_H
#define	SERIAL_H

#include <stdint.h>
#include <stdbool.h>

bool serial_busy = false;
bool msg_sent = true;
uint16_t msg_index = 0;
uint16_t msg_arr[10];
uint16_t msg_len;
const uint16_t newline[2] = {13, 10};
uint8_t newline_index = 0;
bool tx_ready = false;

uint8_t get_len(int32_t num);

void to_msg_array(uint16_t num);

void serial_tx_start(void);

void serial_tx_stop(void);

#endif	/* SERIAL_H */

