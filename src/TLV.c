/*
 * TLV.c
 *
 *  Created on: 2 nov. 2022
 *      Author: Eugenio Yepes & Pablo Victoria Koruza
 */

#include <Defines.h>

AirData_t data;

void sendTLVtoUART(void)
{
	unsigned char buffer[MAX_SIZE_BUFF];
	int lengthBuffer;
	makeTLV(data, buffer, &lengthBuffer);
	//for (int i = 0; i < lengthBuffer; i++) {
	//        printf("%02X", buffer[i]);
	//    }
	UART0_Send(buffer, lengthBuffer);
}

void makeTLV(AirData_t airData, unsigned char *buffer, int *lengthBuffer)
{
    unsigned char auxBuffer[MAX_SIZE_BUFF] = {0};
    unsigned char auxDataBuffer[10] = {0};
    unsigned char auxDataSize = 0;
    unsigned char checkByte = 0x00;
    uint32_t idx;
    *lengthBuffer = 0;

    //printf("Air data in function %s %.3f\t%.3f\t%.3f\t%.3f\n", __func__, airData.sulfurDioxide, airData.carbonMonoxide, airData.lowerExplosiveLimit, airData.temperature);
    //init byte
    *(auxBuffer + *lengthBuffer) = 0xFF;
    (*lengthBuffer)++;
    // sulfurDioxide
    memcpy(auxBuffer + *lengthBuffer, TAG_SULFUR_DIOXIDE, SIZEOF_TAG(TAG_SULFUR_DIOXIDE));
    *lengthBuffer += SIZEOF_TAG(TAG_SULFUR_DIOXIDE);
    auxDataSize = sprintf((char*)auxDataBuffer, NUM_DECIMALS_FORMAT, airData.sulfurDioxide);
    *(auxBuffer + *lengthBuffer) = auxDataSize;
    (*lengthBuffer)++;
    *lengthBuffer += sprintf((char*)auxBuffer + *lengthBuffer, NUM_DECIMALS_FORMAT, airData.sulfurDioxide);

    memset(auxDataBuffer, 0x00, sizeof(auxDataBuffer));

    // carbonMonoxide
    memcpy(auxBuffer + *lengthBuffer, TAG_CARBON_MONOXIDE, SIZEOF_TAG(TAG_CARBON_MONOXIDE));
    *lengthBuffer += SIZEOF_TAG(TAG_CARBON_MONOXIDE);
    auxDataSize = sprintf((char*)auxDataBuffer, NUM_DECIMALS_FORMAT, airData.carbonMonoxide);
    *(auxBuffer + *lengthBuffer) = auxDataSize;
    (*lengthBuffer)++;
    *lengthBuffer += sprintf((char*)auxBuffer + *lengthBuffer, NUM_DECIMALS_FORMAT, airData.carbonMonoxide);

    memset(auxDataBuffer, 0x00, sizeof(auxDataBuffer));

    // lowerExplosiveLimit
    memcpy(auxBuffer + *lengthBuffer, TAG_LEL, SIZEOF_TAG(TAG_LEL));
    *lengthBuffer += SIZEOF_TAG(TAG_LEL);
    auxDataSize = sprintf((char*)auxDataBuffer, NUM_DECIMALS_FORMAT, airData.lowerExplosiveLimit);
    *(auxBuffer + *lengthBuffer) = auxDataSize;
    (*lengthBuffer)++;
    *lengthBuffer += sprintf((char*)auxBuffer + *lengthBuffer, NUM_DECIMALS_FORMAT, airData.lowerExplosiveLimit);

    memset(auxDataBuffer, 0x00, sizeof(auxDataBuffer));

    // temperature
    memcpy(auxBuffer + *lengthBuffer, TAG_TEMPERATURE, SIZEOF_TAG(TAG_TEMPERATURE));
    *lengthBuffer += SIZEOF_TAG(TAG_TEMPERATURE);
    auxDataSize = sprintf((char*)auxDataBuffer, NUM_DECIMALS_FORMAT, airData.temperature);
    *(auxBuffer + *lengthBuffer) = auxDataSize;
    (*lengthBuffer)++;
    *lengthBuffer += sprintf((char*)auxBuffer + *lengthBuffer, "%.3f", airData.temperature);

    // set a check byte to send
    for (idx = 0; idx < *lengthBuffer; idx++) {
    	checkByte ^= auxBuffer[idx];
    }
    *(auxBuffer + *lengthBuffer) = checkByte;
    (*lengthBuffer)++;

    memcpy(buffer, auxBuffer, *lengthBuffer);
}
