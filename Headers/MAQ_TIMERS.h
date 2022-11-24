/*
 * MAQ_TIMERS.h
 *
 *  Created on: 7 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef MAQ_TIMERS_H_
#define MAQ_TIMERS_H_

#define CANTIDAD_TIMER  9

#define TIMER_STATEMACHINE_ContextMenu  0
#define TIMER_KEYBOARD_Sampler			1

#define TIMER_Start_ADC_Conversions 	2
#define TIMER_SO2_ADC_Conversion		3
#define TIMER_CO_ADC_Conversion			4
#define TIMER_EX_ADC_Conversion			5
#define TIMER_TEMP_ADC_Conversion		6
#define TIMER_WakeUp					7
#define TIMER_Alarms_Sentinel			8

//#define TIMER_TLV_Module_Manager		9


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

typedef struct timer
{
    uint32_t Tiempo;
    uint32_t T_Repeticion;
    uint8_t Fin_Tiempo;
    void (*Func)(void);
}timer_t;

//--------------------------------------------------------------------------
//------------PROTOTIPOS DE FUNCIONES---------------------------------------
//--------------------------------------------------------------------------

/**
 * @brief MAQTIMER_Set
 * fill the structure of timer, create a new timer
 * @param Numero 		number of the timer
 * @param T_Final 		final number to reset the number
 * @param T_Repeticion	number of repetitions
 * @param PFunc			function pointer, execute this function when ends the timer only void void function
 */
void MAQTIMER_Set(uint32_t Numero, uint32_t T_Final, uint32_t T_Repeticion, void (*PFunc)(void));
void MAQTIMER_Descontar_Tiempo(void);
void MAQTIMER_Analizar_Tiempo(void);
void MAQTIMER_Timer_Stop(uint32_t Numero);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* MAQ_TIMERS_H_ */
