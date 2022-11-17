/*
 * SysTick.h
 *
 *  Created on: Jul 9, 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <Defines.h>

#define FREC_SYSTEM 	(12000000)  //Micro a 12 MHz
#define FREC_SYSTICK 	(1000)		// Systick interrumpe a 1 kHz
#define DIVISOR_1000mS	(1000)	    //Para config el systick cada 1 seg
#define DIVISOR_100mS	(100)	    //Para config el systick cada 100 mseg
#define DIVISOR_60mS	(60)	    //Para config el systick cada 60 mseg
#define DIVISOR_50mS	(50)	    //Para config el systick cada 50 mseg
#define DIVISOR_10mS	(10)	    //Para config el systick cada 10 mseg

uint32_t Inicializar_SysTick( uint32_t ticks );

#endif /* DRIVER_TIMER_TIMER_DRIVER_H_ */
