/*
 * LCD.c
 *
 *  Created on: Nov. 25, 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#include <Defines.h>


void Demora (uint32_t demoras)
{
	uint32_t i = 0;
	for(i = 0; i <= demoras; i++){}
}
void LCD_PULSO_E (void)
{
	GPIO_SET(LCD_E,  LCD_ALTO);
	Demora(DEMORA_E);
	GPIO_SET(LCD_E,  LCD_BAJO);
	Demora(DEMORA_E);
}
void LCD_Init_salidas_8bits (void)
{
	GPIO_DIR(LCD_RS,OUTPUT);
	//GPIO_DIR(LCD_RW,OUTPUT);
	GPIO_DIR(LCD_E,OUTPUT);
	GPIO_DIR(LCD_D0,OUTPUT);
	GPIO_DIR(LCD_D1,OUTPUT);
	GPIO_DIR(LCD_D2,OUTPUT);
	GPIO_DIR(LCD_D3,OUTPUT);
	GPIO_DIR(LCD_D4,OUTPUT);
	GPIO_DIR(LCD_D5,OUTPUT);
	GPIO_DIR(LCD_D6,OUTPUT);
	GPIO_DIR(LCD_D7,OUTPUT);
}
void LCD_init_8bits (void)
{
	uint32_t i;
	LCD_Init_salidas_8bits();
	for(i=0; i<5; i++)
	{
		Demora(500);
		LCD_SET_8bits();
		LCD_Write_8bits(0,LCD_COMMAND_DISPLAY_ON_CURSOR_BLINK_ON);
		LCD_Write_8bits(0,LCD_COMMAND_ENTRY_MODE_SET);
		LCD_Write_8bits(0, LCD_COMMAND_CLEAR_DISPLAY);
		LCD_Write_8bits(0, LCD_COMMAND_RETURN_HOME);
	}
}

void LCD_SET_8bits(void)
{

	GPIO_SET(LCD_D7, LCD_BAJO);
	GPIO_SET(LCD_D6, LCD_BAJO);
	GPIO_SET(LCD_D5, LCD_ALTO); // Tiene que estar en alto.

	GPIO_SET(LCD_D4, LCD_SET_DL); // DL = 1 para 8 bits
	GPIO_SET(LCD_D3, LCD_SET_N);  // N= 0 para 1 linea.
	GPIO_SET(LCD_D2, LCD_SET_F); // F =0  para 5x11 dots, para 5x8 dots F=1.
	GPIO_SET(LCD_D1, LCD_BAJO); //X Dont Care
	GPIO_SET(LCD_D0, LCD_BAJO); //X Dont Care

	GPIO_SET(LCD_RS, LCD_BAJO);
	//GPIO_SET(LCD_RW, LCD_BAJO);

	LCD_PULSO_E();
}

void LCD_SET_DDRAM (uint32_t fila)
{
	//GPIO_SET(LCD_RW,0);
	GPIO_SET(LCD_RS,0);

		GPIO_SET(LCD_D7, LCD_ALTO ); //Si o si tiene que estar en alto.
		GPIO_SET(LCD_D6, (fila>>6) & 1);
		GPIO_SET(LCD_D5, (fila>>5)  & 1 );
		GPIO_SET(LCD_D4, (fila>>4)  & 1 );

		GPIO_SET(LCD_D3, (fila>>3) & 1 );
		GPIO_SET(LCD_D2, (fila>>1) & 1 );
		GPIO_SET(LCD_D1, (fila>>2) & 1 );
		GPIO_SET(LCD_D0, (fila>>0) & 1 );

		Demora(500); //1800

		LCD_PULSO_E();
}

void LCD_Move_16x2 (uint32_t fila, uint32_t columna)
{
	if(1 == fila)
	{
		LCD_SET_DDRAM(0);
	}
	if(2 == fila)
	{
		LCD_SET_DDRAM(64);
	}
	LCD_Cursor_Move(columna, CURSOR_DERECHA);
}

void LCD_Cursor_Move (uint8_t posicion, uint8_t valor)
{
	uint8_t i=0;
	if(posicion>0)
	{
		for(i=0; i<posicion; i++)
		{
			GPIO_SET(LCD_RS, LCD_BAJO);
			//GPIO_SET(LCD_RW, LCD_BAJO);

			GPIO_SET(LCD_D7, LCD_BAJO);
			GPIO_SET(LCD_D6, LCD_BAJO);
			GPIO_SET(LCD_D5, LCD_BAJO);
			GPIO_SET(LCD_D4, LCD_ALTO);

			if(valor==CURSOR_IZQUIERDA)
			{
				GPIO_SET(LCD_D3, LCD_BAJO); //S/C = 0
				GPIO_SET(LCD_D2, LCD_BAJO); // R/L =0
				GPIO_SET(LCD_D1, LCD_BAJO); //Dont Care.
				GPIO_SET(LCD_D0, LCD_BAJO); //Dont Care.
				LCD_PULSO_E();
			}
			if(valor==CURSOR_DERECHA)
			{
				GPIO_SET(LCD_D3, LCD_BAJO); // S/C = 0
				GPIO_SET(LCD_D2, LCD_ALTO); // R/L = 1
				GPIO_SET(LCD_D1, LCD_BAJO); //Dont Care.
				GPIO_SET(LCD_D0, LCD_BAJO); //Dont Care.
				LCD_PULSO_E();
			}

		}

	}
	if(posicion==0)
	{
		LCD_Write_8bits(posicion, LCD_COMMAND_RETURN_HOME);
	}
}

void LCD_Write_8bits (char valor, char tipo)
{
	uint32_t i;
	switch(tipo)
	{
	case LCD_DATA:
				{
					LCD_Init_salidas_8bits();
					Demora(30);
					GPIO_SET(LCD_RS, LCD_ALTO);
					//GPIO_SET(LCD_RW, LCD_BAJO);
					for(i=0; i<3;i++)
					{
						GPIO_SET(LCD_D7, (valor>>7) & 0x01);
						GPIO_SET(LCD_D6, (valor>>6) & 0x01);
						GPIO_SET(LCD_D5, (valor>>5) & 0x01);
						GPIO_SET(LCD_D4, (valor>>4) & 0x01);

						GPIO_SET(LCD_D3, (valor>>3) & 0x01 );
						GPIO_SET(LCD_D2, (valor>>2) & 0x01 );
						GPIO_SET(LCD_D1, (valor>>1) & 0x01 );
						GPIO_SET(LCD_D0, (valor>>0) & 0x01 );

						Demora(30); //200
					}

					GPIO_SET(LCD_E,  LCD_ALTO);
					GPIO_SET(LCD_E,  LCD_BAJO);

					Demora(DEMORA_E);

					break;
				}

	case LCD_COMMAND_CLEAR_DISPLAY:
				{
					//GPIO_SET(LCD_RW, LCD_BAJO);
					GPIO_SET(LCD_RS, LCD_BAJO);
					GPIO_SET(LCD_D0, LCD_ALTO);
					GPIO_SET(LCD_D1, LCD_BAJO);
					GPIO_SET(LCD_D2, LCD_BAJO);
					GPIO_SET(LCD_D3, LCD_BAJO);
					GPIO_SET(LCD_D4, LCD_BAJO);
					GPIO_SET(LCD_D5, LCD_BAJO);
					GPIO_SET(LCD_D6, LCD_BAJO);
					GPIO_SET(LCD_D7, LCD_BAJO);

					LCD_PULSO_E();

					break;
				}
	case LCD_COMMAND_RETURN_HOME:
				{
					//GPIO_SET(LCD_RW, LCD_BAJO);
					GPIO_SET(LCD_RS, LCD_BAJO);
					GPIO_SET(LCD_D0, LCD_BAJO); //Dont care.
					GPIO_SET(LCD_D1, LCD_ALTO);
					GPIO_SET(LCD_D2, LCD_BAJO);
					GPIO_SET(LCD_D3, LCD_BAJO);
					GPIO_SET(LCD_D4, LCD_BAJO);
					GPIO_SET(LCD_D5, LCD_BAJO);
					GPIO_SET(LCD_D6, LCD_BAJO);
					GPIO_SET(LCD_D7, LCD_BAJO);

					LCD_PULSO_E();

				}
	case LCD_COMMAND_DISPLAY_ON_CURSOR_BLINK_ON:
				{
					//GPIO_SET(LCD_RW, LCD_BAJO);
					GPIO_SET(LCD_RS, LCD_BAJO);

					GPIO_SET(LCD_D7, LCD_BAJO);
					GPIO_SET(LCD_D6, LCD_BAJO);
					GPIO_SET(LCD_D5, LCD_BAJO);
					GPIO_SET(LCD_D4, LCD_BAJO);
					GPIO_SET(LCD_D3, LCD_ALTO); //Tiene que estar en Alto

					GPIO_SET(LCD_D2, LCD_DISPLAY_DISPLAY); //D = 1 muestra el mensaje.
					GPIO_SET(LCD_D1, LCD_DISPLAY_CURSOR); //C = 1 cursor encendido
					GPIO_SET(LCD_D0, LCD_DISPLAY_BLINK); //B = 1 Cursor titilando

					LCD_PULSO_E();

					break;
				}
	case LCD_COMMAND_ENTRY_MODE_SET:
				{
					//GPIO_SET(LCD_RW, LCD_BAJO);
					GPIO_SET(LCD_RS, LCD_BAJO);
					GPIO_SET(LCD_D7, LCD_BAJO);
					GPIO_SET(LCD_D6, LCD_BAJO);
					GPIO_SET(LCD_D5, LCD_BAJO);
					GPIO_SET(LCD_D4, LCD_BAJO);
					GPIO_SET(LCD_D3, LCD_BAJO);
					GPIO_SET(LCD_D2, LCD_ALTO); //Este tiene que estar en alto para activar el Entry mode set.

					GPIO_SET(LCD_D1, LCD_ENTRY_MODE_I_D); // I/D = Si esta en 1, se incrementa la DDRAM a derecha.
					GPIO_SET(LCD_D0, LCD_ENTRY_MODE_S); // S = Si este esta en 1, la DDRAM Escribe.

					LCD_PULSO_E();

					break;
				}
	}
}

void LCD_Full_Display(char* msg1, char* msg2)
{
	uint8_t i = 0;
	uint8_t line = 1;

	LCD_Write_8bits (0, LCD_COMMAND_CLEAR_DISPLAY);

	while(line <=  2)
	{
		if(1 == line)
		{
			LCD_Write_8bits (0, LCD_COMMAND_RETURN_HOME);

			LCD_Move_16x2(line, 1);
			for( i = 0 ; msg1[i]!='\0'; i++ )
			{
				LCD_Write_8bits( msg1[i] , LCD_DATA);
			}
			line++;
		}
		else
		{
			LCD_Write_8bits (0, LCD_COMMAND_RETURN_HOME);

			LCD_Move_16x2(line , 1);
			for( i = 0 ; msg2[i]!='\0'; i++ )
			{
				LCD_Write_8bits( msg2[i] , LCD_DATA);
			}
		line++;
		}
	}
}

void LCD_Display(int8_t* msg, uint32_t fila, uint32_t columna)
{
	uint8_t i=0;

	LCD_Write_8bits (0, LCD_COMMAND_RETURN_HOME);

	LCD_Move_16x2(fila,columna);
	for( i = 0 ; msg[i]!='\0'; i++ )
	{
		LCD_Write_8bits( msg[i] , LCD_DATA);
		Demora(200);
	}
}

