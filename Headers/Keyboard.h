/*
 * Keyboard.h
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

//#include "Defines.h"

#define RELEASE	1       // Para el teclado
#define PRESS 	0       // Para el teclado

 //PROTOTIPOS DE FUNCIONES
void KEYBOARD_Sampler (void);

//--------------------------------------------------------------------------
//-------Estructura del teclado---------------------------------------------

typedef struct
{
    uint32_t UP;
    uint32_t EDIT;
    uint32_t NEXT;
}STRUCT_Keyboard;

#define TECLA_UP	1
#define TECLA_EDIT	2
#define TECLA_NEXT	3

extern STRUCT_Keyboard Keyb_Buffer;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* KEYBOARD_H_ */
