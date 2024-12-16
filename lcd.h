#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

uint16_t water_temp;
float water_temp_float;
uint8_t ambient_temp;
float ambient_temp_float;

void LCD_Init(void);

void update_lcd(void);

#endif	/* LCD_H */

