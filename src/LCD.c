/*
 * LCD.c
 *
 *  Created on: Nov 4, 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

//int FLAG_LCD = 0;

void LCD_Init(void)
{
	GPIO_DIR(LCD_RS, OUTPUT);  	  //Configuro como salidas todos los pines del LCD
	GPIO_SET(LCD_RS, 0);

	GPIO_DIR(LCD_E,  OUTPUT);
	GPIO_SET(LCD_E,  0);

	GPIO_DIR(LCD_D7, OUTPUT);
	GPIO_SET(LCD_D7, 0);

	GPIO_DIR(LCD_D6, OUTPUT);
	GPIO_SET(LCD_D6, 0);

	GPIO_DIR(LCD_D5, OUTPUT);
	GPIO_SET(LCD_D5, 0);

	GPIO_DIR(LCD_D4, OUTPUT);
	GPIO_SET(LCD_D4, 0);

	GPIO_DIR(LCD_D3, OUTPUT);
	GPIO_SET(LCD_D3, 0);

	GPIO_DIR(LCD_D2, OUTPUT);
	GPIO_SET(LCD_D2, 0);

	GPIO_DIR(LCD_D1, OUTPUT);
	GPIO_SET(LCD_D1, 0);

	GPIO_DIR(LCD_D0, OUTPUT);
	GPIO_SET(LCD_D0, 0);

	//GPIO_SET(LCD_D5, 1);
	//GPIO_SET(LCD_D4, 1);

	LCD_Delay();
	LCD_Delay();
	LCD_Delay();
	LCD_Delay();

	LCD_Write(LCD_CONTROL ,  0x3C); // Inicialización del LCD 16x2 en modo 8 bits
	LCD_Delay();
	LCD_Write(LCD_CONTROL ,  0x0C);	// Display ON Cursor OFF
	LCD_Delay();
	LCD_Write(LCD_CONTROL ,  0x01);	// Auto Increment cursor
	LCD_Delay();
	LCD_Write(LCD_CONTROL ,  0x06);	// Clear display

}


void LCD_Write(uint8_t control, uint8_t data)
{
	GPIO_SET(LCD_D0, data & 0x01);
	GPIO_SET(LCD_D1, (data >> 1) & 0x01);
	GPIO_SET(LCD_D2, (data >> 2) & 0x01);
	GPIO_SET(LCD_D3, (data >> 3) & 0x01);
	GPIO_SET(LCD_D4, (data >> 4) & 0x01);
	GPIO_SET(LCD_D5, (data >> 5) & 0x01);
	GPIO_SET(LCD_D6, (data >> 6) & 0x01);
	GPIO_SET(LCD_D7, (data >> 7) & 0x01);

	GPIO_SET(LCD_RS, control);

	GPIO_SET(LCD_E, 1);
	//GPIO_SET(RW, LCD_DATA);   No lo uso porque solamente voy a escribir y no leer
	LCD_Delay();
	GPIO_SET(LCD_E, 0);

}


void LCD_Display(uint8_t *msg , uint8_t row , uint8_t pos)
{
	LCD_Write(LCD_CONTROL, 0x01);
	unsigned char i;

	if(row)
        pos = pos + 0xC0;
	else
		pos = pos + 0x80;

	LCD_Write(0, pos);

	for(i = 0 ; msg[i] != '\0' ; i++) {
		LCD_Write(LCD_DATA, msg[i]);
	}
	msg = '\0';
}


void LCD_Delay(void)
{
  uint32_t 	i;
  for (i = 0 ; i < 600 ; i++ );    //Mínimo 500 sino hace cualquiera el LCD
}

