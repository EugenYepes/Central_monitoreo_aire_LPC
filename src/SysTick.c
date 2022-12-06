/*
 * SysTick.c
 *
 *  Created on: Jul 9, 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

#define MAX_TICKS 	0xffffff

uint32_t Inicializar_SysTick(uint32_t ticks)
{
	if (ticks > MAX_TICKS)
		return 1;      //* Reload value impossible

	SysTick->LOAD = ticks - 1;   		         // set reload register
	SysTick->VAL = 0;                            // Lo inicializo en cero. Es el valor actual
	SysTick->CTRL = (1 << 0) | (1 << 1) | (1 << 2);			 // Enable SysTick IRQ and SysTick Timer
	return 0;
}


void SysTick_Handler(void)
{
	static uint32_t Divisor_t1000 = DIVISOR_1000mS;
	static uint32_t Divisor_t50 = DIVISOR_50mS;
	static uint8_t FLAG_1000mS = 0;
	Divisor_t1000 --;
	Divisor_t50 --;


	uint8_t Tecla = TECLADO_Barrido_HW();  //Barrido del teclado y antirrebote cada 1 ms
	TECLADO_Barrido_SW(Tecla);
	MAQTIMER_Descontar_Tiempo();    // Ejecución de Maquinaria de Timers
	MeasMode_Sentinel();            // Chequea cada 1 ms el modo de medición y maneja el LED STATUS


//-------------------------------------------------------------------------------------	50 mS
	if(0 == Divisor_t50)
	{
		Divisor_t50 = DIVISOR_50mS;
		STATEMACHINE_ContextMenu();    //Corre la Maq de Estados
	}

//-------------------------------------------------------------------------------------	1000 mS
	if(0 == Divisor_t1000)
	{
		Divisor_t1000 = DIVISOR_1000mS;

		TLV_Module_Manager(); 		//Lo ejecuto acá porque es muy pesado para la maquina de timers

		if(0 == FLAG_1000mS)
		{
			FLAG_1000mS = 1;
			GPIO_SET(LED_AZUL, ON);
		}
		else
		{
			FLAG_1000mS = 0;
			GPIO_SET(LED_AZUL, OFF);
		}
	}
}

