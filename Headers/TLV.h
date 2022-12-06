/*
 * TLV.h
 *
 *  Created on: 2 Nov. 2022
 *      Author: Pablo Victoria Koruza & Eugenio Yepes
 */

#ifndef TLV_H_
#define TLV_H_

#include "Defines.h"


//----------DEFINES DE MODULO TLV ------------------------------------------
#include <string.h>

#define ENABLED					1
#define DISABLED				0

#define TAG_SULFUR_DIOXIDE 		"\x5F\x00"
#define TAG_CARBON_MONOXIDE 	"\x5F\x01"
#define TAG_LEL 				"\x5F\x02"
#define TAG_TEMPERATURE 		"\x5F\x03"

#define REQUEST_DATE 			'1'
#define TAG_REQUEST 			"\x5F\x04"
#define TAG_DATE_TIME 			"\x5F\x05"

#define MAX_SIZE_BUFF 			50

#define SIZEOF_TAG(data) (sizeof(data)/sizeof(*data)) - 1

#define NUM_DECIMALS_FORMAT 	"%.3f"

//ESTRUCTURAS Y VARIABLES
//---------Estructuras de sensores------------------------------------------

typedef struct AirData {
    float sulfurDioxide;
    float carbonMonoxide;
    float lowerExplosiveLimit;
    float temperature;
} AirData_t;

//--------PROTOTIPOS DE FUNCIONES TLV---------------------------------------

void sendTLVtoUART(void);
void sendTLVtoUART_request(void);
void Rx_TLVParser(void);
void makeTLV(AirData_t airData, unsigned char *buffer, int *lengthBuffer);
int makeTLVrequest(unsigned char *buffer, int *lengthBuffer);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


#endif /* TLV_H_ */
