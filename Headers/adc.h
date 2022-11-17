/*
 * DRIVER_ADC.h
 *
 *  Created on: Nov 5, 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef ADC_H_
#define ADC_H_

//----DEFINES DEL ADC-----------------------------------

#define MAX_CUENTA 4095

#define TEMP_ADC_Channel  3
#define SO2_ADC_Channel   10
#define CO_ADC_Channel    2
#define EX_ADC_Channel    4

//----VARIABLES GLOBALES DEL ADC------------------------

extern uint32_t	ADC_Cuentas;

extern float Measure_CO;
extern float Measure_SO2;
extern float Measure_EX;
extern float Measure_TEMP;

//----PROTOTIPOS DE FUNCIONES DEL ADC-------------------

void ADC_Init(void);
void ADC_Disparo(uint32_t Canal);


#endif /* ADC_H_ */
