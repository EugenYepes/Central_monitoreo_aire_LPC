/*
===============================================================================
 Name        : main.c
 Author      : Pablo Victoria Koruza
 Version     : 07-07-2022
 Copyright   :
 Description : Multigas Monitor
===============================================================================
*/

#include <Defines.h>

/*size = sprintf(Buf,">T=%04d;ID=%04d;*?<", temp, ID);
for( i = 0, i < 18 ; i++)
	{
	Chk += Buf[i];
	Buf[18] = Chk;
	}

*/
void General_Init(void)
{
	GPIO_Init();
	Inicializar_SysTick(FREC_SYSTEM / FREC_SYSTICK);  //La frec de clock sobre la frec que deseo que funque el systick
	UART0_Init(9600); //Inicializa UART0 a 9600 baudios
	ADC_Init();
	LCD_Init();
	//MAQTIMER_Set (TIMER_STATEMACHINE_ContextMenu, 150, 150, STATEMACHINE_ContextMenu);  //Decidi no setear aca porque se veian raras las variables en el LCD
	MAQTIMER_Set (TIMER_KEYBOARD_Sampler, 15, 15, KEYBOARD_Sampler);                      //Seteo Timer para ejecución de KEYBOARD_Sampler cada 10 mS
	Unit_WakeUp();   //Inicialización de la aplicación
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

