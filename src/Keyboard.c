/*
 * Keyboard.c
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

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
	static uint8_t teclaAnterior = 0;
	static uint8_t cont = 0;

	if(Tecla_Actual == NO_KEY)
	{
		cont = 0;
		teclaAnterior = Tecla_Actual;
		return;
	}

	if (Tecla_Actual == teclaAnterior)
	{
		if(cont > CANTIDAD_VALIDA_REBOTE)
			return;

		if(cont == CANTIDAD_VALIDA_REBOTE)
		{
			Tecla_Filtrada = Tecla_Actual;
		}

		cont++;
	}
	else
	{
		cont = 0;
		teclaAnterior = Tecla_Actual;
	}
}

uint8_t getKey(void)
{
	uint8_t Temporal = Tecla_Filtrada;
	Tecla_Filtrada = NO_KEY;
	return Temporal;
}


uint8_t getKey_Multi(void)
{
	uint8_t Tecla_Anterior = NO_KEY;
	uint8_t Tecla_Actual = NO_KEY;

	uint32_t i = 0;

	Tecla_Anterior = TECLADO_Barrido_HW();

	do
	{
		Tecla_Actual = TECLADO_Barrido_HW();
		i++;
	}
	while (Tecla_Anterior != NO_KEY && Tecla_Anterior == Tecla_Actual && i < VELOCIDAD_TECLA_MULTI);

	return Tecla_Actual;
}



