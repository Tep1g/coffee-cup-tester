#include "lcd.h"
#include "serial.h"
#include <xc.h>
#include <stdbool.h>
#include <math.h>

// --------- LCD_PortB.C ---------------------
//
// This routine has LCD driver routines
//
//  LCD_INST:   send an instruction to the LCD
//  LCD_DATA:   send data to the LCD
//  LCD_INIT:   initialize the LCD to 16x4 mode
//
//--- Revision History -----------------
//     5/20/00   JSG
//     9/27/00   Modify LCD_HELLO for Jump messages
//    11/05/00   Clean up LCD routine to use less RAM
//    10/14/14   Modified for new PIC boards

static const unsigned char CLR16[16] = "                ";
/*static const unsigned char CLR5[5] = "     ";
static const unsigned char CLR8[8] = "        ";*/
static const unsigned char TEMP[3] = "T: "; //    "T: 123  Amb: 65 "
static const unsigned char AMB[5] = "Amb: "; // "126.1   -12.1522"
/*static const unsigned char a_txt[2] = "a ";
static const unsigned char b_txt[2] = "b ";*/
static bool first_temp_taken = false;
static int32_t BTB[2][2] = {{0, 0}, {0, 0}};
static float BTY[2][1] = {{0.0f}, {0.0f}};
static uint32_t lcd_calls = 0;
static uint32_t time_sec = 0;

static void Wait_ms(unsigned int X)
{
   unsigned int i, j;

   for (i=0; i<X; i++)
      for (j=0; j<617; j++);
   }


static void LCD_Pause(void)
{
   unsigned char x;
   for (x=0; x<20; x++);
   }


static void LCD_Strobe(void)
{
   RB3 = 0;
   LCD_Pause();
   RB3 = 1;
   LCD_Pause();
   RB3 = 0;
   LCD_Pause();
   }
         
//  write a byte to the LCD in 4 bit mode 

static void LCD_Inst(unsigned char c)
{
    RB2 = 0;        // send an instruction
	PORTB = (PORTB & 0x0F) |  (c & 0xF0);
	LCD_Strobe();
	PORTB = (PORTB & 0x0F) |  ((c<<4) & 0xF0);
	LCD_Strobe();
	Wait_ms(10);
}

static void LCD_Move(unsigned char Row, unsigned char Col)
{
   if (Row == 0) LCD_Inst(0x80 + Col);
   if (Row == 1) LCD_Inst(0xC0 + Col);
   if (Row == 2) LCD_Inst(0x94 + Col);
   if (Row == 3) LCD_Inst(0xD4 + Col);
   }
   
static void LCD_Write(unsigned char c)
{
    RB2 = 1;        // send data
	PORTB = (PORTB & 0x0F) |  (c & 0xF0);
	LCD_Strobe();
	PORTB = (PORTB & 0x0F) |  ((c<<4)  & 0xF0);
	LCD_Strobe();
	
}


static void LCD_Out(long int DATA, unsigned char D, unsigned char S)
{
   unsigned char A[10], i;
   
   if(DATA < 0) {
      LCD_Write('-');
      DATA = -DATA;
      }
   else LCD_Write(' ');
   for (i=0; i<10; i++) {
      A[i] = DATA % 10;
      DATA = DATA / 10;
   }
   for (i=D; i>0; i--) {
      if (i == S) LCD_Write('.');
      LCD_Write(A[i-1] + '0');
   }
}

void LCD_Init(void)
{
   TRISB = 0;       
   RB1 = 0;  
   LCD_Inst(0x33);
   LCD_Inst(0x32);
   LCD_Inst(0x28);
   LCD_Inst(0x0E);
   LCD_Inst(0x01);
   LCD_Inst(0x06);
   Wait_ms(100);
   
   LCD_Move(0,0); for (uint8_t i=0; i<16; i++) LCD_Write(CLR16[i]);
   LCD_Move(1,0); for (uint8_t i=0; i<16; i++) LCD_Write(CLR16[i]);
   LCD_Move(0,0); for (uint8_t i=0; i<3; i++) LCD_Write(TEMP[i]);
   LCD_Move(0,8); for (uint8_t i=0; i<5; i++) LCD_Write(AMB[i]);
   /*LCD_Move(1,0); for (uint8_t i=0; i<2; i++) LCD_Write(a_txt[i]);
   LCD_Move(1,0); for (uint8_t i=0; i<2; i++) LCD_Write(a_txt[i]);*/
   }
/*
static void multiplyMatrices(float mat1[2][2], float mat2[2][1], uint8_t m1, uint8_t n1, uint8_t n2, float res[2][1]) {
  for (uint8_t i = 0; i < m1; i++) {
    for (uint8_t j = 0; j < n2; j++) {
      res[i][j] = 0;
      for (uint8_t k = 0; k < n1; k++) {
        res[i][j] += mat1[i][k] * mat2[k][j];
      }
    }
  }
}
*/
void update_lcd(void) {
    LCD_Move(0,2); for (uint8_t i=0; i<4; i++) LCD_Write(CLR16[i]);
    LCD_Move(0,2);
    LCD_Out(water_temp, get_len((int32_t)water_temp), 0);
    
    LCD_Move(0,12); for (uint8_t i=0; i<4; i++) LCD_Write(CLR16[i]);
    LCD_Move(0,12);
    LCD_Out(ambient_temp, get_len((int32_t)ambient_temp), 0);
    
    lcd_calls = (lcd_calls + 1)%10;
    
    if (lcd_calls == 0) {
        //water_temp_float = get_water_temp();
        //ambient_temp_float = get_ambient_temp();
        time_sec += 8; //this piece of code is executed every 8 seconds
    
        BTB[0][0] += (time_sec * time_sec); //sum of x^2
        BTB[0][1] += time_sec; //sum of x
        BTB[1][0] = BTB[0][1]; //sum of x
        BTB[1][1]++; //n

        float ln_temp = logf(water_temp_float-ambient_temp_float); //y = ln(T - T_amb)
        BTY[0][0] += (time_sec * ln_temp); //sum of x*y
        BTY[1][0] += ln_temp; //sum of y
        tx_ready = true;

        if ((water_temp > (ambient_temp + 10)) && (first_temp_taken)) { //commented out for test purposes
        //if ((water_temp > ambient_temp) && (first_temp_taken)) { //ensures we only calculate if we have at least 2 points

            //float determinant_factor = 1/(float)((BTB[0][0]*BTB[1][1])-(BTB[0][1]*BTB[1][0]));
            float determinant = (float)((BTB[0][0]*BTB[1][1])-(BTB[0][1]*BTB[1][0]));
            float inv_BTB[2][2]; // (B_transpose * B)^-1
            
            inv_BTB[0][0] = BTB[1][1]/determinant;
            inv_BTB[0][1] = -(BTB[0][1])/determinant;
            inv_BTB[1][0] = -(BTB[1][0])/determinant;
            inv_BTB[1][1] = BTB[0][0]/determinant;
            /*
            inv_BTB[0][0] = r1c1;
            inv_BTB[0][1] = r1c2;
            inv_BTB[1][0] = r2c1;
            inv_BTB[1][1] = r2c2;
            */
            float A[2][1];

            A[0][0] = (inv_BTB[0][0] * BTY[0][0]) + (inv_BTB[0][1] * BTY[1][0]); // A = (BTB)^-1 * BTY
            A[1][0] = (inv_BTB[1][0] * BTY[0][0]) + (inv_BTB[1][1] * BTY[1][0]);

            //int32_t b = roundf(A[0][0] * 10000.0f); //A[0][0] is -b
            int32_t ttc = roundf(-1/(A[0][0]));
            int32_t a = roundf(expf(A[1][0])*10); //A[1][0] is log(a))

            //uint32_t ttc = (time_ms/(log(a) - log(water_temp - avg_amb_temp)))/1000; //THIS IS b^-1 (ttc in seconds)
            LCD_Move(1,8); for (uint8_t i=0; i<8; i++) LCD_Write(CLR16[i]);
            LCD_Move(1,8);
            //LCD_Out(b, get_len(b), 0);
            LCD_Out(ttc, get_len(ttc), 0);

            LCD_Move(1,0); for (uint8_t i=0; i<5; i++) LCD_Write(CLR16[i]);
            LCD_Move(1,0);
            LCD_Out(a, get_len(a), 1);
        }

        else if(!first_temp_taken) {
            first_temp_taken = true;
        }
    }
}