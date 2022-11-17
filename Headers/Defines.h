/*
 * Defines.h
 *
 *  Created on: Jul 9, 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef DEFINES_H_
#define DEFINES_H_

typedef signed char 	int8_t;
typedef unsigned char 	uint8_t;
typedef short 			int16_t;
typedef unsigned short 	uint16_t;
typedef int 			int32_t;
typedef unsigned int 	uint32_t;

#include <cr_section_macros.h>
#include <stdio.h>

#include <LPC845.h>
#include <GPIO.h>
#include <MAQ_TIMERS.h>
#include <adc.h>
#include <TLV.h>
#include <LCD.h>
#include <Keyboard.h>
#include <ExternalModules.h>
#include <GasMonitor_App.h>
#include <SysTick.h>
#include <UART0_Driver.h>
#include <core_cm0plus.h>

void General_Init(void);

uint8_t TECLADO_Barrido_HW(void);
void TECLADO_Barrido_SW(uint8_t Tecla_Actual);
uint8_t GetKey(void);

#define 	NO_KEY		0xff

#endif /* DEFINES_H_ */
