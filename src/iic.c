// =========================================================================================
//  Modulo: i2c_driver.C
//  Proposito: Funciones de lectura y grabacion de memorias IIC
//  Fecha: 26-07-2011
//  Autor: Ing. Gustavo Fresno
//=========================================================================================

#include "Defines.h"

//-------------------------------------------
void IIC_Inicializacion(void)
{
	// Configuro los pines del micro como salida
	GPIO_DIR( SDA, 1 );
	GPIO_DIR( SCL, 1 );
	// Seteo los pines
	GPIO_SET( SDA, 1 );
	GPIO_SET( SCL, 1 );

	// Envio señales de Start
	IIC_Demora(TIEMPO_IIC);

	GPIO_SET(SDA, 0);

	IIC_Demora(TIEMPO_IIC);

	GPIO_SET(SCL, 0);
}

void IIC_Start(void)
{
	// Configuro los pines del micro como salida
//	GPIO_DIR(SDA, 1);
//	GPIO_DIR(SCL, 1);

	// Seteo los pines
	GPIO_SET(SDA, 1);
	GPIO_SET(SCL, 1);

	// Envio señales de Start
	IIC_Demora(TIEMPO_IIC);

	GPIO_SET(SDA, 0);

	IIC_Demora(TIEMPO_IIC);

	GPIO_SET(SCL, 0);

	IIC_Demora(TIEMPO_IIC);
}
//--------------------------------------------------------------
void IIC_Stop(void)
{
//	uint32_t	i;

	GPIO_SET(SDA, 0);
	GPIO_SET(SCL, 1);

	IIC_Demora(TIEMPO_IIC);

	GPIO_SET(SDA, 1);

	IIC_Demora(TIEMPO_IIC);
}

//--------------------------------------------------------------
// Funcion donde se envia el clock y se realiza la lectura del pin SDA
//--------------------------------------------------------------
uchar IIC_Read_Clock(void)
{
	uchar Level;         	/* linea de SDA */
	
	IIC_Demora(TIEMPO_IIC_1);

	// Seteo el pin CLK
	GPIO_SET(SCL, 1);

	IIC_Demora(TIEMPO_IIC_1);
	// Se configura como entrada, para realizar la lectura
	GPIO_DIR(SDA, 0);

	IIC_Demora(TIEMPO_IIC_1);

	// Realizo la lectura del SDA
	Level = GPIO_READ(SDA);

	//Demora_FOR(TIEMPO_IIC_1);
	//GPIOSetValue(SCL, 1);

	IIC_Demora(TIEMPO_IIC_1);
	// Se configura SDA como salida nuevamente
	GPIO_DIR(SDA, 1);
	GPIO_SET(SDA, 1);

	IIC_Demora(TIEMPO_IIC_1);

	GPIO_SET(SCL, 0);

	IIC_Demora(TIEMPO_IIC_1);

	return(Level);
}

//--------------------------------------------------------------
// Funcion donde se envia el clock
//--------------------------------------------------------------
void IIC_Clock(void)
{
	IIC_Demora(TIEMPO_IIC_1);

	GPIO_SET(SCL, 1);

	IIC_Demora(TIEMPO_IIC_1);

	GPIO_SET(SCL, 0);

	IIC_Demora(TIEMPO_IIC_1);
}

//--------------------------------------------------------------
// Funcion donde se envia un byte a la memoria
//--------------------------------------------------------------
uchar IIC_Write(uchar byte)
{
	uchar 	mask = 0x80;
	
	while(mask)
	{
		if (byte & mask)
			GPIO_SET(SDA, 1);
		else
			GPIO_SET(SDA, 0);

		IIC_Clock();
		mask >>= 1;
	}
	GPIO_SET(SDA, 1);
	return(IIC_Read_Clock());
}
//--------------------------------------------------------------
// Funcion donde se lee un byte de la memoria
//--------------------------------------------------------------
uchar IIC_Read(uchar acknowledgment)
{
	uchar mask = 0x80,byte = 0x00;
	
	while(mask)
	{
		if (IIC_Read_Clock())
			byte |= mask;
		mask >>= 1;
	}

	if(acknowledgment)
	{
		// Esto es para lectura consecutivas
		GPIO_SET(SDA, 0);
		IIC_Clock();
		GPIO_SET(SDA, 1);
	}
	else
	{
		GPIO_SET(SDA, 1);
		IIC_Clock();
	}
	return(byte);
}

/*
void IIC_Write_Driver_32b(uint Direccion, uchar Memoria, uint32_t Dato)
{
	STRUCT_INT32b	Dato_Temporal;

	Dato_Temporal.Uint32b = Dato;

	IIC_Write_Driver(Direccion, Memoria, Dato_Temporal.Bytes.Byte[0]);
	IIC_Write_Driver(Direccion + 1, Memoria, Dato_Temporal.Bytes.Byte[1]);
	IIC_Write_Driver(Direccion + 2, Memoria, Dato_Temporal.Bytes.Byte[2]);
	IIC_Write_Driver(Direccion + 3, Memoria, Dato_Temporal.Bytes.Byte[3]);
}

uint32_t IIC_Read_Driver_32b(uint Direccion, uchar Memoria)
{
	STRUCT_INT32b	Dato_Temporal;

	Dato_Temporal.Bytes.Byte[3] = (uint32_t)IIC_Read_Driver(Direccion + 3, Memoria);
	Dato_Temporal.Bytes.Byte[2] = (uint32_t)IIC_Read_Driver(Direccion + 2, Memoria);
	Dato_Temporal.Bytes.Byte[1] = (uint32_t)IIC_Read_Driver(Direccion + 1, Memoria);
	Dato_Temporal.Bytes.Byte[0] = (uint32_t)IIC_Read_Driver(Direccion, Memoria);

	return Dato_Temporal.Uint32b;
}

*/

uint16_t IIC_Read_Driver_16b(uint Direccion, uchar Memoria)
{
	uint16_t	Dato_Temporal;

	Dato_Temporal = (uint8_t)IIC_Read_Driver(Direccion, Memoria);
	Dato_Temporal *= 256;
	Dato_Temporal += (uint8_t)IIC_Read_Driver(Direccion + 1, Memoria);

	return Dato_Temporal;
}

void IIC_Write_Driver_16b(uint Direccion, uchar Memoria, uint16_t Dato)
{
	IIC_Write_Driver(Direccion, Memoria, (uint8_t)(Dato / 256));
	IIC_Write_Driver(Direccion + 1, Memoria, (uint8_t)(Dato % 256));
}

/*
//--------------------------------------------------------------
// Funcion para escribir una posicion de memoria
// Parametros:
//	Direccion: es una direccion de la memoria desde 0 a TAMANIO_MEMORIA
//	Memoria: es la direccion fisica de la memoria
// 	byte: dato a grabar en la memoria
//--------------------------------------------------------------*/
//uint8_t 	status;
uchar IIC_Write_Driver(uint Direccion, uchar Memoria, uchar byte)
{
	uchar 	Reintento = 0;

	//status = 0;               /* falla por default */

	do
	{
		//NVIC->ICER[0] = (1 << 16);

		IIC_Start();
		if (!IIC_Write(Memoria))
		{
			//status = 1;
			if (!IIC_Write((unsigned char)(Direccion >> 8)))
			{
				//status = 2;
				if (!IIC_Write((unsigned char)(Direccion)))
				{
					//status = 3;
					if (!IIC_Write(byte))
					{
						//status = 4;
						IIC_Stop();
						//NVIC->ISER[0] = (1 << 16);
						return 1;          /* success */
					}
				}
			}
		}
		IIC_Stop();

		//NVIC->ISER[0] = (1 << 16);
		Reintento++;

	}while(Reintento < CANTIDAD_REINTENTOS_I2C);

	return(0);
}
//--------------------------------------------------------------
// Funcion para leer una posicion de memoria
// Parametros:
//	Direccion: es una direccion de la memoria desde 0 a TAMANIO_MEMORIA
//	Memoria: es la direccion fisica de la memoria
// Devuelve: uchar dato leido
//--------------------------------------------------------------
uchar IIC_Read_Driver(uint Direccion, uchar Memoria)
{
	uchar	Dato_Leido = 0xff, Reintento = 0;
	
	do
	{
		//NVIC->ICER[0] = (1 << 16);
		IIC_Start();
		if (!IIC_Write(Memoria))
		{
			if (!IIC_Write((unsigned char)(Direccion >> 8)))
			{
				if (!IIC_Write((unsigned char)Direccion))
				{
					IIC_Start();
					if (!IIC_Write(Memoria | 0x01))
					{
						Dato_Leido = IIC_Read(NO_ACK);
						IIC_Stop();
						//NVIC->ISER[0] = (1 << 16);
						return(Dato_Leido);
					}
				}
			}
		}
		IIC_Stop();
		//NVIC->ISER[0] = (1 << 16);
		Reintento++;

	}while(Reintento < CANTIDAD_REINTENTOS_I2C);

	return(Dato_Leido);
}

void IIC_Leer_Bloque(uint32_t Direccion, uint8_t* Datos, uint32_t Tamanio)
{
	uint32_t	i;

	for(i = 0 ; i < Tamanio ; i++)
	{
		Datos[i] = (uint8_t)IIC_Read_Driver(Direccion + i, MEMORIA_1);
	}
}

void IIC_Guardar_Bloque(uint32_t Direccion, uint8_t* Datos, uint32_t Tamanio)
{
	uint32_t	i;

	for(i = 0 ; i < Tamanio ; i++)
	{
		IIC_Write_Driver(Direccion + i, MEMORIA_1, Datos[i]);
	}
}

//--------------------------------------------------------------
// Funcion para tener una demora
//--------------------------------------------------------------
void IIC_Demora(uint32_t demora)
{
	uint32_t	tiempo;

	for(tiempo = 0; tiempo < demora ; tiempo++);
	for(tiempo = 0; tiempo < demora ; tiempo++);
}					

//--------------------------------------------------------------
// Funcion para borrar toda las memorias del log
//--------------------------------------------------------------
void Borrar_Memoria(uchar Memoria)
{
	uint32_t	i;

	for( i = 0 ; i < TAMANIO_MEMORIA ; i++)
	{
		IIC_Write_Driver(i , Memoria, 6 );
	}
}

