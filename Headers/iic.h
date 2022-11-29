// =========================================================================================
//  Modulo: iic.C
//  Proposito: Funciones de lectura y grabacion de memorias IIC
//  Fecha: 26-07-2011
//  Autor: Ing. Gustavo Fresno
//=========================================================================================

#ifndef IIC_
#define IIC_

#define NO_ACK  					0     /* lack of acknowledgment          */

#define	TIEMPO_IIC					300  //Tiempos entre grabacion
#define	TIEMPO_IIC_1				100   // Tiempo para la lectura de un cambio de estado en el puerto

#define	CANTIDAD_REINTENTOS_I2C		5

#define	SDA							0,11
#define	SCL							0,10

#define	TAMANIO_MEMORIA				(4096 * 8)
#define MEMORIA_1					0   //Lo agregue yo para que compile

#define	uchar						uint8_t
#define	uint						uint32_t
#define	ulong						uint32_t

void IIC_Start(void);
void IIC_Stop(void);
uchar IIC_Read_Clock(void);
void IIC_Clock(void);
uchar IIC_Write(uchar );
uchar IIC_Read(uchar );
uchar IIC_Escribir(uint , uchar , uchar );
uchar IIC_Leer(uint , uchar );
void IIC_Demora(uint32_t demora);
void Borrar_Memoria(uchar);
void IIC_Inicializacion(void);


uchar IIC_Write_Driver(uint Direccion, uchar Memoria, uchar byte);
uchar IIC_Read_Driver(uint Direccion, uchar Memoria);

//void IIC_Write_Driver_32b(uint Direccion, uchar Memoria, uint32_t Dato);
//uint32_t IIC_Read_Driver_32b(uint Direccion, uchar Memoria);

uint16_t IIC_Read_Driver_16b(uint Direccion, uchar Memoria);
void IIC_Write_Driver_16b(uint Direccion, uchar Memoria, uint16_t Dato);

void IIC_Leer_Bloque(uint32_t Direccion, uint8_t* Datos, uint32_t Tamanio);
void IIC_Guardar_Bloque(uint32_t Direccion, uint8_t* Datos, uint32_t Tamanio);

#endif /* IIC_ */
