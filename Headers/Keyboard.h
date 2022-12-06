/*
 * Keyboard.h
 *
 *  Created on: Nov. 5 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

//---------DEFINES DE TECLADO-----------------------------------------------

#define TECLA_UP					1
#define TECLA_EDIT					2
#define TECLA_NEXT					3
#define PRESS 						0       // Para el teclado
#define NO_KEY						0xFF
#define CANTIDAD_VALIDA_REBOTE 		30		//Cantidad de muestras a tomar para considerar un estado valido por antirebote
#define VELOCIDAD_TECLA_MULTI		4000    //Cuanto más alto este valor, mas lenta la tecla Multi

//---------PROTOTIPOS DE FUNCIONES DE TECLADO -----------------------------

uint8_t TECLADO_Barrido_HW(void);
void TECLADO_Barrido_SW(uint8_t Tecla_Actual);
uint8_t getKey(void);
uint8_t getKey_Multi(void);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* KEYBOARD_H_ */
