/*
 * ExternalModules.c
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>


void EXTERNALMODULES_Lo_Alarm(uint32_t command)
{
	if(ON == command || OFF == command) { GPIO_SET(LO_ALARM_OUTPUT, command); }	//AL Alarma baja
}

void EXTERNALMODULES_Hi_Alarm(uint32_t command)
{
	if(ON == command || OFF == command) { GPIO_SET(HI_ALARM_OUTPUT, command); }	//AL Alarma baja
}

void EXTERNALMODULES_Status_Leds(uint32_t command)
{
	if(ONLINE == command)
	{
		GPIO_SET(LED_STATUS_ONLINE, ON);	//LED STATUS ONLINE
		GPIO_SET(LED_STATUS_OFFLINE, OFF);	//LED STATUS OFFLINE
	}
	if(OFFLINE == command)
	{
		GPIO_SET(LED_STATUS_ONLINE, OFF);	//LED STATUS ONLINE
		GPIO_SET(LED_STATUS_OFFLINE, ON);	//LED STATUS OFFLINE
	}
}


