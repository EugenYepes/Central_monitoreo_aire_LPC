/*
 * ExternalModules.h
 *
 *  Created on: Nov. 5 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#ifndef EXTERNALMODULES_H_
#define EXTERNALMODULES_H_

//--------DEFINES DE FUNCIONES DE MODULOS EXTERNOS-----------------------

#define ONLINE 		1
#define OFFLINE 	0

//--------PROTOTIPOS DE FUNCIONES DE MODULOS EXTERNOS-----------------------

void EXTERNALMODULES_Lo_Alarm(uint32_t command); 		//Driver para activar y desactivar Alarma Baja
void EXTERNALMODULES_Hi_Alarm(uint32_t command);		//Driver para activar y desactivar Alarma Alta
void EXTERNALMODULES_Status_Leds(uint32_t command);		//Driver para control de Led de Status
void EXTERNALMODULES_Clock(void);						//Driver de reloj por software

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#endif /* EXTERNALMODULES_H_ */
