/*
 * UART0_Driver.h
 *
 *  Created on: Jul 26, 2019
 *      Author: GJF-Trabajo
 */

#ifndef DRIVER_UART_UART0_DRIVER_H_
#define DRIVER_UART_UART0_DRIVER_H_

#define		UART0_TAMANIO_COLA_RX					50
#define		UART0_TAMANIO_COLA_TX					50

typedef struct
{
	uint8_t						Datos_Validos;
	uint16_t					Tamanio;
	uint8_t						Buffer[UART0_TAMANIO_COLA_RX];
	uint16_t					Indice_in, Indice_out;
}STRUCT_RX0;

typedef struct
{
	uint8_t						Datos_Validos;
	uint16_t					Tamanio;
	uint8_t						Buffer[UART0_TAMANIO_COLA_TX];
	uint16_t					Indice_in, Indice_out;
}STRUCT_TX0;

typedef struct
{
	STRUCT_RX0					RX;
	STRUCT_TX0					TX;
}UART0_Struct;

/**
 * @brief UART0_Init
 * initialization wart for serial communication
 * @param baudrate speed of serial communication
 */
void UART0_Init(uint32_t baudrate);
void UART0_Send(uint8_t *Datos, uint32_t Tamanio);
void UART0_PushTx(uint8_t Dato);
int32_t UART0_PopTx( void );
void UART0_PushRx(uint8_t Dato);
int8_t UART0_PopRx(uint8_t *dato);
//int32_t UART0_PopRx( void );

#endif /* DRIVER_UART_UART0_DRIVER_H_ */
