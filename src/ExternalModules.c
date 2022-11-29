/*
 * ExternalModules.c
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

extern UnitParameters_t Unit_Data;  //Para uso del reloj interno por software
extern uint8_t ExternalClock;

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

void EXTERNALMODULES_Clock(void)
{
	if(ENABLED == ExternalClock)
	{
	uint32_t Leap_Year_Calc;    //Uso para calcular año bisiesto

	Unit_Data.Data.Second ++;

	if(Unit_Data.Data.Second == 60)
	{
		Unit_Data.Data.Second = 0;
		Unit_Data.Data.Minute ++;
	}

	if(Unit_Data.Data.Minute == 60)
	{
		Unit_Data.Data.Minute = 0;
		Unit_Data.Data.Hour ++;
	}

	if(Unit_Data.Data.Hour == 24)
	{
		Unit_Data.Data.Hour = 0;

		if((Unit_Data.Data.Month % 2) == 0)    //Antes de incrementar un día, analizo la situación en el calendario
		{
			if (Unit_Data.Data.Month < 7)
			{
	//-------------------------- COMIENZO DEL MOTOR DE CALCULO DE AÑOS BISIESTOS----------------------------------------------------------------
				if(Unit_Data.Data.Month == 2)
				{
						Leap_Year_Calc = Unit_Data.Data.Year % 10;   //Extrae la unidad

						if((Unit_Data.Data.Year % 2) != 0)     //Descarto años impares
						{
							if(Unit_Data.Data.Day < 28) Unit_Data.Data.Day ++;
							else
							{
								Unit_Data.Data.Day = 1;
								if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
								else
								{
									Unit_Data.Data.Month = 1;
									if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
									else Unit_Data.Data.Year = 0;
								}
							}
						}
						else
						{

							if( 0 == Leap_Year_Calc || 4 == Leap_Year_Calc || 8 == Leap_Year_Calc)  //Si la unidad es par y 0 || 4 || 8
							{
								Leap_Year_Calc = (Unit_Data.Data.Year - Leap_Year_Calc) / 10;     //Extrae la decena

								if((Leap_Year_Calc % 2) == 0) //Siendo la unidad 0 || 4 || 8 y la decena par ES BISIESTO
								{
									if(Unit_Data.Data.Day < 29) Unit_Data.Data.Day ++;
									else
									{
										Unit_Data.Data.Day = 1;
										if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
										else
										{
											Unit_Data.Data.Month = 1;
											if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
											else Unit_Data.Data.Year = 0;
										}
									}
								}
								else
								{
									if(Unit_Data.Data.Day < 28) Unit_Data.Data.Day ++;
									else
									{
										Unit_Data.Data.Day = 1;
										if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
										else
										{
											Unit_Data.Data.Month = 1;
											if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
											else Unit_Data.Data.Year = 0;
										}
									}
								}
							}

							else    //Si la unidad es par y 2 || 6
							{
								Leap_Year_Calc = (Unit_Data.Data.Year - Leap_Year_Calc) / 10;     //Extrae la decena

								if((Leap_Year_Calc % 2) != 0) //Siendo la unidad 2 || 6 y la decena impar ES BISIESTO
								{
									if(Unit_Data.Data.Day < 29) Unit_Data.Data.Day ++;
									else
									{
										Unit_Data.Data.Day = 1;
										if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
										else
										{
											Unit_Data.Data.Month = 1;
											if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
											else Unit_Data.Data.Year = 0;
										}
									}
								}
								else
								{
									if(Unit_Data.Data.Day < 28) Unit_Data.Data.Day ++;
									else
									{
										Unit_Data.Data.Day = 1;
										if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
										else
										{
											Unit_Data.Data.Month = 1;
											if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
											else Unit_Data.Data.Year = 0;
										}
									}
								}

							}
						}

					}
	//-------------------------- FIN DEL MOTOR DE CALCULO DE AÑOS BISIESTOS--------------------------------------------------------------
				else
				{
					if(Unit_Data.Data.Day < 30) Unit_Data.Data.Day ++;
					else
					{
						Unit_Data.Data.Day = 1;
						if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
						else
						{
							Unit_Data.Data.Month = 1;
							if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
							else Unit_Data.Data.Year = 0;
						}
					}
				}
			}
			else
			{
				if(Unit_Data.Data.Day < 31) Unit_Data.Data.Day ++;
				else
				{
					Unit_Data.Data.Day = 1;
					if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
					else
					{
						Unit_Data.Data.Month = 1;
						if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
						else Unit_Data.Data.Year = 0;
					}
				}
			}
		}

		if( (Unit_Data.Data.Month % 2) != 0 )
		{
			if(Unit_Data.Data.Month <= 7)
			{
				if(Unit_Data.Data.Day < 31) Unit_Data.Data.Day ++;
				else
				{
					Unit_Data.Data.Day = 1;
					if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
					else
					{
						Unit_Data.Data.Month = 1;
						if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
						else Unit_Data.Data.Year = 0;
					}
				}
			}
			else
			{
				if(Unit_Data.Data.Day < 30) Unit_Data.Data.Day ++;
				else
				{
					Unit_Data.Data.Day = 1;
					if(Unit_Data.Data.Month < 12) Unit_Data.Data.Month ++;
						else
						{
							Unit_Data.Data.Month = 1;
							if(Unit_Data.Data.Year < 99) Unit_Data.Data.Year ++;
							else Unit_Data.Data.Year = 0;
						}
					}
				}
			}
		}
	}
}


