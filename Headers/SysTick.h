/*
 * SysTick.h
 *
 *  Created on: Jul 9, 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <Defines.h>

#define FREC_SYSTEM 	(12000000)  //Micro a 12 MHz
#define FREC_SYSTICK 	(1000)		//Systick interrumpe a 1 kHz
#define DIVISOR_1000mS	(1000)	    //Para config el systick cada 1 seg
#define DIVISOR_50mS	(50)	    //Para config el systick cada 50 mseg

//------ PROTOTIPOS DE FUNCIONES SYSTICK ------------

uint32_t Inicializar_SysTick( uint32_t ticks );

#endif /* SYSTICK_H_ */
