/*
 * GPIO.c
 *
 *  Created on: 7 jul. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

void GPIO_Init(void)
{
	SYSCON->SYSAHBCLKCTRL0 |= (1<<6)  //Enciende el clock del GPIO0
							| (1<<18) //Enciende el clock del registro IOCON
							| (1<<20) //Enciende el clock del GPIO1
							| (1<<7); //Enciende el clock del switch matrix

	IOCON-> PIO[IOCON_INDEX_PIO0_16] |= (2<<3) ; //PULL-UP interna para P0.16
	IOCON-> PIO[IOCON_INDEX_PIO0_17] |= (2<<3) ; //PULL-UP interna para P0.17
	IOCON-> PIO[IOCON_INDEX_PIO0_18] |= (2<<3) ; //PULL-UP interna para P0.18

	//GPIO_DIR(puerto, pin, configuración)
	//GPIO_SET(puerto, pin, estado)

	GPIO_DIR(0, 16, INPUT);  	//Tecla NEXT (PIO0_16)
	GPIO_DIR(0, 17, INPUT);  	//Tecla EDIT (PIO0_17)
	GPIO_DIR(0, 18, INPUT);  	//Tecla UP (PIO0_18)

	GPIO_DIR(1, 1,  OUTPUT);  	//LED AZUL DE ACTIVIDAD
	GPIO_DIR(0, 19, OUTPUT);	//AL Alarma baja
	GPIO_DIR(0, 20, OUTPUT);	//AH Alarma alta
	GPIO_DIR(0, 21, OUTPUT);	//LED STATUS ONLINE
	GPIO_DIR(0, 24, OUTPUT);	//LED STATUS OFFLINE


	//GPIO_SET(1, 1, OFF);	 //Amanece apagado


}

void GPIO_DIR (uint32_t port, uint32_t pin, uint32_t mode)
{
	if(mode == OUTPUT)
		GPIO -> DIRSET [port] = (1 << pin); //ver linea 2281 de LPC845.h
	else
		GPIO -> DIRCLR [port] = (1 << pin); //ver linea 2283 de LPC845.h
}

void GPIO_SET (uint32_t port, uint32_t pin, uint32_t state)
{
	if(state == ON)
		GPIO -> SET [port] |= (1 << pin); //ver linea 2275 de LPC845.h
	else
		GPIO -> CLR [port] |= (1 << pin); //ver linea 2277 de LPC845.h
}

uint32_t GPIO_READ(uint32_t port, uint32_t pin)
{
	uint32_t Lectura;
	Lectura = GPIO->PIN[port];
	Lectura = (Lectura >> pin ) & 1 ;
	return Lectura;
}

/*uint32_t GPIO_READ (uint32_t port, uint32_t pin)
{
	uint32_t	Lectura;
	if(GPIO -> PIN [port] &= (1 << pin)); //ver linea 2271 de LPC845.h
	//Aca hago una mascara para filtrar lo que hay en el pin, en el puerto seleccionado.
	//En el if no necesito compararlo con nada. En ese caso si existe entra y si no existe es else.
}*/


STRUCT_Entradas Entrada[CANTIDAD_ENTRADAS];  //Estructura con todas las entradas que uso con el antirebote
uint32_t i;//porque global?

void GPIO_Antirebote (void)
{
	uint8_t Estado_Actual;
	for(i = 0 ; i < CANTIDAD_ENTRADAS ; i++)
	{
		switch (i)
		{
			case 0:
				Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_0);
				break;
			case 1:
				Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_1);
				break;
			case 2:
				Estado_Actual = GPIO_READ (GPIO_PULSADOR_EXTERNO_2);
				break;
		}

		if (Estado_Actual == Entrada[i].Estado_Anterior)
		{
			Entrada[i].Cont++;

			if(Entrada[i].Cont >= CANTIDAD_VALIDA_REBOTE)
			{
				Entrada[i].Estado_Filtrado = Estado_Actual;
			}
		}
		else
		{
			Entrada[i].Cont = 0;
			Entrada[i].Estado_Anterior = Estado_Actual;
		}
	}
}


uint32_t GPIO_Leer_Filtrada (uint32_t Numero)
{
	return Entrada[Numero].Estado_Filtrado;
}


