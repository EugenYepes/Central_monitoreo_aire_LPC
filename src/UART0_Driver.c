/*
 * UART_Driver.c
 *
 *  Created on: May 17, 2019
 *      Author: GJF-Trabajo
 */

#include 	"Defines.h"

UART0_Struct		UART0;

uint32_t Enviando_Datos = 0;

/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/

void UART0_Init(uint32_t baudrate)
{
	// HABILITO LA UART0
	SYSCON->SYSAHBCLKCTRL0 |= (1 << 14);					// 14 = UART0

	// CONFIGURO LA MATRIX
	//SWM0->PINASSIGN.PINASSIGN0 = (8 << 0) | (9 << 8);		// TX0 = P0.8	RX0 = P0.9
	//SWM0->PINASSIGN.PINASSIGN0 = (10 << 0) | (11 << 8);	// TX0 = P0.10	RX0 = P0.11
	SWM0->PINASSIGN.PINASSIGN0 = (27 << 0) | (26 << 8);		// TX0 = P0.27	RX0 = P0.26

	// CONFIGURACION GENERAL
	USART0->CFG = 	(0 << 0)		// 0=DISABLE 1=ENABLE
					| (1 << 2)		// 0=7BITS 1=8BITS 2=9BITS
					| (0 << 4)		// 0=NOPARITY 2=PAR 3=IMPAR
					| (0 << 6)		// 0=1BITSTOP 1=2BITSTOP
					| (0 << 9)		// 0=NOFLOWCONTROL 1=FLOWCONTROL
					| (0 << 11)		// 0=ASINCRONICA 1=SINCRONICA
					| (0 << 14);	// 0=SLAVE 1=MASTER

	// CONFIGURACION EXTRA
	USART0->CTL = 0;

	// CONFIGURACION INTERRUPCIONES
	USART0->INTENSET = (1 << 0);	//RX Y TX INTERRUPT

	// CONFIGURACION DEL BAUDRATE
	UART0CLKSEL = 1;

	USART0->BRG = (FREC_SYSTEM / baudrate) / 16;

	// CONFIGURACION INTERRUPCIONES GLOBALES
	NVIC->ISER[0] = (1 << 3); /* enable interrupt */

	// ENABLE LA UART
	USART0->CFG |= 	(1 << 0);			// ENABLE USART0
}

void UART0_PushTx(uint8_t dato)
{
	UART0.TX.Buffer[UART0.TX.Indice_in] = dato;

	UART0.TX.Indice_in ++;
	UART0.TX.Indice_in %= UART0_TAMANIO_COLA_TX;

	if ( UART0.TX.Datos_Validos == 0 )
	{
		UART0.TX.Datos_Validos = 1;

		// fuerzo el envio del primer byte
		USART0->TXDAT = (uint8_t)UART0_PopTx();

		USART0->INTENSET = (1 << 2); // int tx
	}
}

int32_t UART0_PopTx(void)
{
	int32_t dato = -1;

	if ( UART0.TX.Indice_in != UART0.TX.Indice_out )
	{
		dato = (int32_t) UART0.TX.Buffer[UART0.TX.Indice_out];
//		UART0.TX.Buffer[UART0.TX.Indice_out] = 0;

		UART0.TX.Indice_out ++;
		UART0.TX.Indice_out %= UART0_TAMANIO_COLA_TX;
	}
	return dato;
}

void UART0_PushRx(uint8_t dato)
{
	UART0.RX.Buffer[UART0.RX.Indice_in] = dato;

	UART0.RX.Indice_in ++;
	UART0.RX.Indice_in %= UART0_TAMANIO_COLA_RX;
}

int8_t UART0_PopRx(uint8_t *dato)
{
	int8_t status = -1;

	if ( UART0.RX.Indice_in != UART0.RX.Indice_out )
	{
		*dato = UART0.TX.Buffer[UART0.RX.Indice_out];
		UART0.RX.Indice_out ++;
		UART0.RX.Indice_out %= UART0_TAMANIO_COLA_RX;
	}
	return status;
}

/*
int32_t UART0_PopRx(void)
{
	int32_t dato = -1;

	if ( UART0.RX.Indice_in != UART0.RX.Indice_out )
	{
		dato = (int32_t) UART0.TX.Buffer[UART0.RX.Indice_out];
		UART0.RX.Indice_out ++;
		UART0.RX.Indice_out %= UART0_TAMANIO_COLA_RX;
	}
	return dato;
}
*/

//----------------------------------------------------

/*#define 	CANTIDAD_DATOS	10

uint32_t 	in = 0;
uint32_t 	out = 0;
uint8_t 	Buf[CANTIDAD_DATOS];

void PUSH_RX(uint8_t Dato)
{
	Buf[in] = Dato;
	
	in++;
	if(in > CANTIDAD_DATOS)
		in = 0;
	
	//in %= CANTIDAD_DATOS;    es otra forma de escribirlo
}

void PUSH_TX(uint8_t Dato)
{
	Buf[in] = Dato;

	in++;
	if(in > CANTIDAD_DATOS)
		in = 0;

	//in %= CANTIDAD_DATOS;    es otra forma de escribirlo
}


int16_t POP_TX(void)      //Ojo, es signado (int16_t) porque cuando no haya nada en el buffer devuelve -1
{
	uint8_t 	Temporal;
	
	if(in == out)
		return -1;
	
	Temporal = Buf[out];
	
	out++;
	if(out > CANTIDAD_DATOS)
		out = 0;
	
	return Temporal;
}


//OTRA FORMA DE HACER POP CON PUNTEROS. ES VERSATIL PORQUE NO DEVUELVE EL DATO SINO EL ESTADO SI FUE POPEADO O NO

/*uint8_t POP_RX(uint8_t *Dato)
{
	uint8_t 	Temporal;
	
	if(in == out)
		return 0;
	
	*Dato = Buf[out];
	
	out++;
	if(out > CANTIDAD_DATOS)
		out = 0;
	
	return 1;
}
*/


void UART0_Send(uint8_t *data, uint32_t sizeData)
{
	uint32_t 	i;
	int16_t 	Dato_Temporal;
	
	
	for(i = 0 ; i < sizeData ; i++)
	{
		UART0_PushTx(data[i]);
	}
	
	if(Enviando_Datos == 0)
	{
		Enviando_Datos = 1;
		
		Dato_Temporal = UART0_PopTx();
		if(Dato_Temporal >= 0)
		{
			// Fuerzo el envio del 1er byte
			
			USART0->TXDAT = (uint8_t)Dato_Temporal;
			
			USART0->INTENSET = (1 << 2); // enable int tx
		}
	}
}

void UART0_IRQHandler(void)
{
	int16_t 	Dato_Temporal;
	
	uint32_t 	Status = USART0->INTSTAT;
	
	if(Status & (1 << 0))
	{
		//Interrupcion RX
		Dato_Temporal = (int16_t)USART0->RXDAT;
		
		UART0_PushRx(Dato_Temporal);
	}
	
	if(Status & (1 << 2))
	{
		//Interrupcion TX
		Dato_Temporal = UART0_PopTx();
		if(Dato_Temporal >= 0)
		{ 
			USART0->TXDAT = (uint8_t)Dato_Temporal;
		}
		else
		{
			USART0->INTENCLR = (1 << 2); // int tx
			Enviando_Datos = 0;
		}
	}
}


































