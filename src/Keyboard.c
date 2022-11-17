/*
 * Keyboard.c
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

STRUCT_Keyboard Keyb_Buffer;

uint8_t Tecla_Filtrada = NO_KEY;

uint8_t TECLADO_Barrido_HW(void)
{
	uint8_t Estado_Actual;

	Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_0);
	if(0 == Estado_Actual)
		return TECLA_UP;
	Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_1);
	if(0 == Estado_Actual)
		return TECLA_EDIT;
	Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_2);
	if(0 == Estado_Actual)
		return TECLA_NEXT;

	return NO_KEY;
}

void TECLADO_Barrido_SW(uint8_t Tecla_Actual)
{
	static uint8_t Tecla_Anterior = 0;
	static uint8_t Cont = 0;

	if(Tecla_Actual == NO_KEY)
	{
		Cont = 0;
		Tecla_Anterior = Tecla_Actual;
		return;
	}

	if (Tecla_Actual == Tecla_Anterior)
	{
		if(Cont > CANTIDAD_VALIDA_REBOTE)
			return;

		if(Cont == CANTIDAD_VALIDA_REBOTE)
		{
			Tecla_Filtrada = Tecla_Actual;
		}

		Cont++;
	}
	else
	{
		Cont = 0;
		Tecla_Anterior = Tecla_Actual;
	}
}

uint8_t GetKey(void)
{
	uint8_t Temporal = Tecla_Filtrada;
	Tecla_Filtrada = NO_KEY;
	return Temporal;
}

void KEYBOARD_Sampler (void)
{
	Keyb_Buffer.UP = GPIO_Leer_Filtrada (PULSADOR_EXTERNO_0);
	Keyb_Buffer.EDIT = GPIO_Leer_Filtrada (PULSADOR_EXTERNO_1);
	Keyb_Buffer.NEXT = GPIO_Leer_Filtrada (PULSADOR_EXTERNO_2);
}


