/*
===============================================================================================
 Autores:			Pablo Victoria Koruza y Eugenio Yepes
 Versión:			Ciclo lectivo 2022
 Descripción:		Monitor Multigas de Uso Industrial - Cátedra de Informática II - UTN FRBA
 Curso:				R2052
 Profesor:			Ing. Gustavo Fresno
 Jefes de TP:   	Ing. Mariano Vedovato e Ing. Jorge Manganiello
===============================================================================================
*/

#include <Defines.h>

void General_Init(void)
{
	GPIO_Init();
	Inicializar_SysTick(FREC_SYSTEM / FREC_SYSTICK);  							//La frec de clock sobre la frec que deseo que funque el systick
	UART0_Init(9600); 															//Inicializa la UART0 a 9600 baudios
	ADC_Init();																	//Inicializa módulo ADC
	LCD_init_8bits();															//Inicializa el Display LCD
	MAQTIMER_Set (TIMER_EXTERNALMODULES_Clock, 750, 750 ,EXTERNALMODULES_Clock);//Reloj interno por software se ejecuta c/ 1 seg
	Unit_WakeUp();   															//Inicializa la aplicación
}

int main(void)
{

	General_Init();

	while(1)
	{
		MAQTIMER_Analizar_Tiempo();
    }
	return 0;
}

