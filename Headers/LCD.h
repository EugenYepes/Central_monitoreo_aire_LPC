/*
 * LCD.h
 *
 *  Created on: Nov 4, 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef LCD_H_
#define LCD_H_

#define 	LCD_CONTROL		0
#define 	LCD_DATA		1

//#define		LCD_RW	         No lo uso porque solamente voy a escribir y no leer
#define		LCD_RS		0,9
#define		LCD_E		0,8

#define		LCD_D7		0,0
#define		LCD_D6		0,1
#define		LCD_D5		0,28
#define		LCD_D4		0,29
#define		LCD_D3		0,30
#define		LCD_D2		0,15
#define		LCD_D1		0,6
#define		LCD_D0		0,31

void LCD_Write(uint8_t control, uint8_t data);
void LCD_Display(uint8_t *msg, uint8_t row, uint8_t pos);
void LCD_Init(void);
void LCD_Delay(void);

//extern int FLAG_LCD;


#endif /* LCD_ */
