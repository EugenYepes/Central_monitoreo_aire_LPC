/*
 * TLV.c
 *
 *  Created on: 2 nov. 2022
 *      Author: Eugenio Yepes & Pablo Victoria Koruza
 */

#include <Defines.h>

AirData_t data;
extern uint8_t dataParsed;
extern UnitParameters_t Unit_Data;

void sendTLVtoUART_request(void)
{
	unsigned char buffer[MAX_SIZE_BUFF];
	int lengthBuffer;
	makeTLVrequest(buffer, &lengthBuffer);
	//for (int i = 0; i < lengthBuffer; i++) {
	//        printf("%02X", buffer[i]);
	//    }
	UART0_Send(buffer, lengthBuffer);
}

void Rx_TLVParser(void)
{
	uint8_t tag[10], buff[50], aux[50];
	uint8_t dato;
	uint8_t CheckSumRX = 0;
	uint8_t lengthDataTag = 0;
	static uint8_t tagPos = 0, i = 0;
	static uint8_t Estado = 0;

	if (0 == UART0_PopRx(&dato))
	{
		switch(Estado)
		{
			case 0:
				if(dato == 0xFF)
				{
					dataParsed = 0;
					Estado = 1;
					CheckSumRX = dato;
				}
				break;

			case 1:
				if((dato & 0x1F) == 0x1F)
				{
					tag[tagPos] = dato;
					tagPos++;
					CheckSumRX ^= dato;
					Estado = 2;
				}
				else
					Estado = 0;
				break;

			case 2:

				if((dato & 0x80) == 0x80)
				{
					tag[tagPos] = dato;
					tagPos++;
					CheckSumRX ^= dato;
				}
				else
				{
					tag[tagPos] = dato;
					CheckSumRX ^= dato;
					Estado = 3;
				}
				break;

			case 3:
				if ((dato & 0x80) != 0x80) {
					if (dato != 0x80) {
						lengthDataTag += dato & 0x7F;
						Estado = 4;
						CheckSumRX ^= dato;
					} else {
						Estado = 0;
						//printf("indefinite length not developed");
					}
				} else
					Estado = 0;
				break;

			case 4:
				if (memcmp(tag, TAG_DATE_TIME, SIZEOF_TAG(TAG_DATE_TIME)) == 0) {
					if (i < lengthDataTag) {
						CheckSumRX ^= dato;
						aux[i] = dato;
						i++;
					}
					else
					{
						Estado = 5;
					}
				}
				break;
			case 5:
				if(CheckSumRX == dato)   //Comparo Checksum calculado por el enviado (ambos son de 8 bits)
				{
					Estado = 0;
					// get date what have format dd.MM.yy hh:mm:ss
					if (memcmp(tag, TAG_DATE_TIME, SIZEOF_TAG(TAG_DATE_TIME)) == 0) {
						for (i = 0; i < lengthDataTag;)
						{
							sprintf(buff, "%c%c", aux[i], aux[i + 1]);
							if (0 == i)
								Unit_Data.Data.Day = atoi(buff);
							if (3 == i)
								Unit_Data.Data.Month = atoi(buff);
							if (6 == i)
								Unit_Data.Data.Year = atoi(buff);
							if (9 == i)
								Unit_Data.Data.Hour = atoi(buff);
							if (12 == i)
								Unit_Data.Data.Minute = atoi(buff);
							if (15 == i)
								Unit_Data.Data.Second = atoi(buff);
							i += 3;
						}
					}
					dataParsed = 1;
				}
				Estado = 0;
				break;
			default :
				break;
		}
	}
}

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

int makeTLVrequest(unsigned char *buffer, int *lengthBuffer)
{
    unsigned char auxBuffer[MAX_SIZE_BUFF] = {0};
    unsigned char auxDataBuffer[10] = {0};
    unsigned char auxDataSize = 0;
    unsigned char checkByte = 0;
    int idx;
    *lengthBuffer = 0;

    *(auxBuffer + *lengthBuffer) = 0xFF;
    (*lengthBuffer)++;

    memcpy(auxBuffer + *lengthBuffer, TAG_REQUEST, SIZEOF_TAG(TAG_REQUEST));
    *lengthBuffer += SIZEOF_TAG(TAG_REQUEST);
    auxDataSize = sprintf((char*)auxDataBuffer, "%c", REQUEST_DATE);
    *(auxBuffer + *lengthBuffer) = auxDataSize;
    (*lengthBuffer)++;
    *lengthBuffer += sprintf((char*)auxBuffer + *lengthBuffer, "%c", REQUEST_DATE);

    // set a check byte to send
    for (idx = 0; idx < *lengthBuffer; idx++) {
        checkByte ^= auxBuffer[idx];
    }
    *(auxBuffer + *lengthBuffer) = checkByte;
    (*lengthBuffer)++;
    //printf("calculated check byte to send %d\n", *(auxBuffer + *lengthBuffer));
    memcpy(buffer, auxBuffer, *lengthBuffer);
    return 0;
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
