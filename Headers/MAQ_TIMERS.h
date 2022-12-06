/*
 * MAQ_TIMERS.h
 *
 *  Created on: Nov 7. 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#ifndef MAQ_TIMERS_H_
#define MAQ_TIMERS_H_

#define CANTIDAD_TIMER  9

typedef enum
{
	TIMER_STATEMACHINE_ContextMenu,
	TIMER_Start_ADC_Conversions,
	TIMER_SO2_ADC_Conversion,
	TIMER_CO_ADC_Conversion,
	TIMER_EX_ADC_Conversion,
	TIMER_TEMP_ADC_Conversion,
	TIMER_WakeUp,
	TIMER_Alarms_Sentinel,
	TIMER_EXTERNALMODULES_Clock
}timers_t;


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

void MAQTIMER_Set(uint32_t Numero, uint32_t T_Final, uint32_t T_Repeticion, void (*PFunc)(void));
void MAQTIMER_Descontar_Tiempo(void);
void MAQTIMER_Analizar_Tiempo(void);
void MAQTIMER_Timer_Stop(uint32_t Numero);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* MAQ_TIMERS_H_ */
