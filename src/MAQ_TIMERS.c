/*
 * MAQ_TIMERS.c
 *
 *  Created on: 7 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

timer_t T[CANTIDAD_TIMER];

void MAQTIMER_Set (uint32_t Numero, uint32_t T_Final, uint32_t T_Repeticion, void (*PFunc)(void))
{
	T[Numero].Tiempo = T_Final;
	T[Numero].Fin_Tiempo = 0;
	T[Numero].Func = PFunc;
	T[Numero].T_Repeticion = T_Repeticion;
}

void MAQTIMER_Descontar_Tiempo(void)
{
	uint32_t i;
	for(i=0 ; i < CANTIDAD_TIMER ; i++)
	{
		if(T[i].Tiempo > 0)
		{
			T[i].Tiempo--;
			if(0 == T[i].Tiempo)
			{
				T[i].Fin_Tiempo = 1;
				if(T[i].T_Repeticion)
				{
					T[i].Tiempo = T[i].T_Repeticion;
				}
			}
		}
	}
}

void MAQTIMER_Analizar_Tiempo(void)
{
	uint32_t i;

	for(i=0 ; i < CANTIDAD_TIMER ; i++)
	{
		if(T[i].Fin_Tiempo)
		{
			T[i].Fin_Tiempo = 0;
			T[i].Func();
		}
	}
}

void MAQTIMER_Timer_Stop(uint32_t Numero)    //Timer killer
{
	T[Numero].Tiempo = 0;
	T[Numero].Fin_Tiempo = 0;
	T[Numero].T_Repeticion = 0;
}
