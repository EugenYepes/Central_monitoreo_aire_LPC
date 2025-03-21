/*
 * GPIO.h
 *
 *  Created on: 7 jul. 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef GPIO_H_
#define GPIO_H_

//#include "Defines.h"


//Defines

#define ON	1
#define OFF	0
#define OUTPUT 	1
#define INPUT 	0

 //PROTOTIPOS DE FUNCIONES GPIO
void GPIO_Init (void);   //Inicializo clock y GPIOs

void GPIO_DIR (uint32_t port, uint32_t pin, uint32_t mode);  //Configuro cuales puertos y pines serán entradas o salidas

void GPIO_SET (uint32_t port, uint32_t pin, uint32_t state);

uint32_t GPIO_READ (uint32_t port, uint32_t pin);

//--------------------------------------------------------------------------
//ANTIREBOTE Y ENUNCIADO DE ENTRADAS GPIO-----------------------------------
//--------------------------------------------------------------------------

#define PULSADOR_EXTERNO_0			0       //Para usar con GPIO_Leer_Filtrada
#define PULSADOR_EXTERNO_1			1       //Para usar con GPIO_Leer_Filtrada
#define PULSADOR_EXTERNO_2			2       //Para usar con GPIO_Leer_Filtrada
#define GPIO_PULSADOR_EXTERNO_0		0,18    //Tecla UP
#define GPIO_PULSADOR_EXTERNO_1		0,17	//Tecla EDIT
#define GPIO_PULSADOR_EXTERNO_2		0,16	//Tecla NEXT

#define LO_ALARM_OUTPUT				0,19	//Salida relé de AL
#define HI_ALARM_OUTPUT		     	0,20	//Salida relé de AH
#define LED_STATUS_ONLINE			0,21
#define LED_STATUS_OFFLINE			0,24

#define CANTIDAD_ENTRADAS 			3       //Cantidad de entradas GPIO para barrer con el antirebote
#define CANTIDAD_VALIDA_REBOTE 		30		//Cantidad de muestras a tomar para considerar un estado valido por antirebote

typedef struct
{
    uint8_t Cont;
    uint8_t Estado_Anterior;
    uint8_t Estado_Filtrado;
}STRUCT_Entradas;

void GPIO_Antirebote (void);
uint32_t GPIO_Leer_Filtrada (uint32_t Numero);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* GPIO_H_ */
