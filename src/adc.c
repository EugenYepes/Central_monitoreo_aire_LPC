/*
 * DRIVER_ADC.c
 *
 *  Created on: Jan 3, 2020
 *      Author: GJF-Trabajo
 */

#include "Defines.h"

uint32_t  ADC_Cuentas = 0;
uint32_t  ADC_LastConversion = 0;
float Raw_ADC = 0;

extern SensorData_t Data_NoFiltered;

float Measure_CO = 0;
float Measure_SO2 = 0;
float Measure_EX = 0;
float Measure_TEMP = 0;

/*****************************************************************************
** Function name:		ADC_Init
**
** Descriptions:		Inicializamos el ADC
**
** parameters:			void
** Returned value:		void
**
*****************************************************************************/
void ADC_Init(void)
{
	SYSCON->PDRUNCFG &= ~(1 << 4);				// Enciendo el modulo del ADC que esta apagado por default
	SYSCON->SYSAHBCLKCTRL0 |= (1 << 24);		// 24 = ADC

	SWM0->PINENABLE0 &= ~(1 << 16);			// ADC_2 enable PIO0_14 (CO)
	SWM0->PINENABLE0 &= ~(1 << 17);			// ADC_3 enable PIO0_23 (Temp)
	SWM0->PINENABLE0 &= ~(1 << 18);			// ADC_4 enable PIO0_22 (Ex)
	SWM0->PINENABLE0 &= ~(1 << 24);			// ADC_10 enable PIO0_13 (SO2)



	ADC0->CTRL 	= (1)							// CLKDIV se recomienda el minimo si no hay una necesidad de velocidad
				| (0 << 8)						// ASYNCMODE
				| (0 << 10)						// LPWRMODE
				| (0 << 30);					// CALMODE

	// A/D Conversion Sequence A Control Register

	ADC0->SEQ_CTRL[0] 	= (0)				// CANAL, lo voy a seleccionar en el momento de disparar la conversion
						| (0 << 12)				// TRIGGER
						| (1 << 18)				// TRIGPOL					0=FLANCO NEGATIVO DE LOS TRIGGERS 1=FLANCO POSITIVO DE LOS TRIGGERS
						| (0 << 19)				// SYNCBYPASS
						| (0 << 26)				// START OF CONVERSION		1=SOC 0=STOP
						| (0 << 29)				// SINGLESTEP
						| (0 << 30)				// MODE						0
						| (1 << 31);			// SEQA_ENA					0=SECUENCIA A DESHABILITADA 1=HABILITADA

	ADC0->INTEN = (1 << 0);						// Sequence A interrupt enable

	NVIC->ISER[0] = (1 << 16); 					/* enable interrupt ISE_ADC_SEQA*/

}

void ADC_Disparo(uint32_t Canal)
{
	ADC_LastConversion = Canal;
	ADC0->SEQ_CTRL[0] 	&= ~(0x0FFF);			// Limpio los bits de seleccion del canal
	ADC0->SEQ_CTRL[0] 	|= (1 << Canal)			// Seleccion del canal
						| (1 << 26);			// START OF CONVERSION		1=SOC 0=STOP
}

void ADC_SEQA_IRQHandler(void)
{
  //uint8_t Buf_ADC[30];
	uint32_t	Valor = ADC0->SEQ_GDAT[0];
  //uint32_t 	Canal = (Temporal >> 26) & 0x0f;
	Valor = Valor >> 4;
	Valor &= 0x0FFF;

	if(ADC_LastConversion == CO_ADC_Channel)
	{
		Raw_ADC = Valor;
		Raw_ADC /= MAX_CUENTA;
		Measure_CO = Raw_ADC * 33;  	  //Fullscale sensor CO
		Measure_EX -= 2;				  //Zero offset del sensor CO

		Data_NoFiltered.CO_Reading = Measure_CO;

		Sensor_DataFilter(CO_ADC_Channel);           //Envío la medición de CO sin filtrar al filtro de ADC
	}

	if(ADC_LastConversion == SO2_ADC_Channel)
	{
		Raw_ADC = Valor;
		Raw_ADC /= MAX_CUENTA;
		Measure_SO2 = Raw_ADC * 33;

		Data_NoFiltered.SO2_Reading = Measure_SO2;

		Sensor_DataFilter(SO2_ADC_Channel);           //Envío la medición de SO2 sin filtrar al filtro de ADC
	}
	if(ADC_LastConversion == EX_ADC_Channel)
	{

		Raw_ADC = Valor;
		Raw_ADC /= MAX_CUENTA;
		Measure_EX = Raw_ADC * 100;  	//Fullscale sensor EX
		Measure_EX -= 8;				//Zero offset del sensor EX

		Data_NoFiltered.EX_Reading = Measure_EX;

		Sensor_DataFilter(EX_ADC_Channel);           //Envío la medición de EX sin filtrar al filtro de ADC
	}
	if(ADC_LastConversion == TEMP_ADC_Channel)
	{
		Raw_ADC = Valor;
		Raw_ADC /= MAX_CUENTA;
		Measure_TEMP = Raw_ADC * 260;      //Fullscale sensor Temp

		Data_NoFiltered.TEMP_Reading = Measure_TEMP;

		Sensor_DataFilter(TEMP_ADC_Channel);           //Envío la medición de TEMP sin filtrar al filtro de ADC
		//printf("Meas: %.2f \n", Measure_T);
		//printf("Data %.2f \n", data.temperature);
	}

}

