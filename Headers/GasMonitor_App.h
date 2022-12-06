/*
 * GasMonitor_App.h
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#ifndef GASMONITOR_APP_H_
#define GASMONITOR_APP_H_

//-----DEFINES DE LOS PARAMETROS DE FABRICA DEL EQUIPO-----------------------------------

#define SERIAL_NUMBER               22021107
#define DEFAULT_ID				    001
#define DEFAULT_TEMP_SPAN_FACTOR    0.87
#define DEFAULT_SO2_ZERO_FACTOR    -0.74
#define DEFAULT_SO2_SPAN_FACTOR		1
#define DEFAULT_CO_ZERO_FACTOR	   -2.10
#define DEFAULT_CO_SPAN_FACTOR      1
#define DEFAULT_EX_ZERO_FACTOR		2
#define DEFAULT_EX_SPAN_FACTOR		1
#define DEFAULT_SO2_LO_ALARM 		5
#define DEFAULT_SO2_HI_ALARM		8
#define DEFAULT_CO_LO_ALARM   		15
#define DEFAULT_CO_HI_ALARM			25
#define DEFAULT_EX_LO_ALARM			15
#define DEFAULT_EX_HI_ALARM			25


//----DEFINES DE LA FUNCIÓN DE FILTRADO DE ADC --------------------------------------------
#define ADC_SAMPLING_PERIOD			   10    //Periodo de muestreo del ADC en mS
#define NUMBER_OF_ADC_SAMPLES          12    //Cantidad de muestras para filtrar


//-----DEFINES DE LA MAQUINA DE ESTADOS DEL MENU CONTEXTUAL -------------------------------

#define ENABLED							1
#define DISABLED						0


typedef enum {
	STATE_INIT,
	MeasModeEN_AlarmsDIS,
	MeasModeEN_AlarmsEN,
	MeasModeDIS_AlarmsDIS_SMODE,
	Settings,
	Show_SO2,
	Show_CO,
	Show_EX,
	Show_Temp,
	Show_SO2_Alarms,
	Show_CO_Alarms,
	Show_EX_Alarms,
	Show_DateTime,
	Show_IDSN,
	Receive_Data,
	Receive_Data_LINK,
	Calibration,
	AlarmsSettings,
	Set_DateTime,
	ID_Settings,
	Set_ID,
	Set_Year,
	Set_Month,
	Set_Day,
	Set_Hour,
	Set_Minute,
	Set_Alarms_SO2,
	Set_Alarm_SO2_LO,
	Set_Alarm_SO2_HI,
	Set_Alarms_CO,
	Set_Alarm_CO_LO,
	Set_Alarm_CO_HI,
	Set_Alarms_EX,
	Set_Alarm_EX_LO,
	Set_Alarm_EX_HI,
	Calibrate_SO2,
	Calibrate_SO2_ZERO,
	Calibrate_SO2_SPAN,
	Calibrate_CO,
	Calibrate_CO_ZERO,
	Calibrate_CO_SPAN,
	Calibrate_EX,
	Calibrate_EX_ZERO,
	Calibrate_EX_SPAN,
	Calibrate_Temp,
	Calibrate_Temp_SPAN
}states_t;
//#define MeasModeEN_AlarmsDIS			1       // ESTADO PRIMARIO
//#define MeasModeEN_AlarmsEN				2
//#define MeasModeDIS_AlarmsDIS_SMODE 	3
//#define Settings						4
//#define Show_SO2 						5
//#define Show_CO 						6
//#define Show_EX 						7
//#define Show_Temp 						8
//#define Show_DateTime 					9
//#define Show_IDSN  						10
//
//#define Calibration 					11
//#define AlarmsSettings					12
//#define Set_DateTime 					13
//#define ID_Settings 					14
//
//#define Set_ID 							15
//
//#define Set_Year 						16
//#define Set_Month 						17
//#define Set_Day 						18
//#define Set_Hour 						19
//#define Set_Minute 						20
//
//#define Set_Alarms_SO2 					21
//#define Set_Alarm_SO2_LO 				22
//#define Set_Alarm_SO2_HI 				23
//#define Set_Alarms_CO 					24
//#define Set_Alarm_CO_LO 				25
//#define Set_Alarm_CO_HI 				26
//#define Set_Alarms_EX 					27
//#define Set_Alarm_EX_LO 				28
//#define Set_Alarm_EX_HI 				29
//
//#define Calibrate_SO2 					30
//#define Calibrate_SO2_ZERO 				31
//#define Calibrate_SO2_SPAN 				32
//#define Calibrate_CO 					33
//#define Calibrate_CO_ZERO 				34
//#define Calibrate_CO_SPAN 				35
//#define Calibrate_EX 					36
//#define Calibrate_EX_ZERO 				37
//#define Calibrate_EX_SPAN 				38
//#define Calibrate_Temp 					39
//#define Calibrate_Temp_SPAN 			40

//----DEFINES DE LA MAQUINA DE ESTADOS PARA DISPARAR LOS 4 CANALES DEL ADC-----------------

#define Start_ADC_Conversion_Timer     	0
#define TEMP_Wait_Start_ADC_Conversion 	1
#define SO2_Wait_Start_ADC_Conversion  	2
#define CO_Wait_Start_ADC_Conversion   	3
#define EX_Wait_Start_ADC_Conversion   	4

//-----PROTOTIPO DE LA FUNCION DE FILTRADO DE ADC -----------------------------------------

void Sensor_DataFilter(uint32_t sensor);

//-----PROTOTIPO DE LA FUNCION DE CORRECCIÓN DE LECTURAS SEGUN FACTORES DE CALIBRACIÓN ----

void Sensor_DataCorrecter(uint32_t sensor);

//-----PROTOTIPO DE LA FUNCION PARA DISPARO DE ALARMAS ------------------------------------

void Alarms_Sentinel(void);

//-----PROTOTIPO DE LA FUNCION DE CONTROL DEL MODO DE MEDICIÓN-----------------------------

void MeasMode_Sentinel(void);

//-----PROTOTIPO DEL MODULO DE TRANSMISIÓN TLV---------------------------------------

void TLV_Module_Manager(void);

//-----PROTOTIPOS DEL MOTOR DE INICIALIZACION DE LA APLICACIÓN ----------------------------

void Unit_WakeUp(void);
void WakeUp_Step1_Hello1(void);
void WakeUp_Step1_Hello2(void);
void WakeUp_Step1_Hello3(void);
void WakeUp_Step1_Hello4(void);
void WakeUp_Step2_Setup(void);
void WakeUp_Step3_SN(void);
void WakeUp_Step4_GasList(void);
void WakeUp_Step5_WarmUp(void);
void WakeUp_Step6_StartUnit(void);

//-----PROTOTIPOS DE LA MAQUINA DE ESTADOS DEL MENU CONTEXTUAL ----------------------------

void STATEMACHINE_ContextMenu(void);
void Setup_Default_Parameters(void);


void STATE_MeasModeEN_AlarmsEN(uint8_t);
void STATE_MeasModeEN_AlarmsDIS(uint8_t);
void STATE_MeasModeDIS_AlarmsDIS_SMODE(uint8_t);
void STATE_Show_DateTime(uint8_t);
void STATE_Show_IDSN(uint8_t);
void STATE_Settings(uint8_t);
void STATE_Show_SO2(uint8_t);
void STATE_Show_CO(uint8_t);
void STATE_Show_EX(uint8_t);
void STATE_Show_Temp(uint8_t);
void STATE_Show_SO2_Alarms(uint8_t Tecla);
void STATE_Show_CO_Alarms(uint8_t Tecla);
void STATE_Show_EX_Alarms(uint8_t Tecla);
void STATE_Calibration(uint8_t);
void STATE_AlarmsSettings(uint8_t);
void STATE_ID_Settings(uint8_t);
void STATE_Set_ID(uint8_t);
void STATE_Set_DateTime(uint8_t);
void STATE_Set_Year(uint8_t, uint8_t);
void STATE_Set_Month(uint8_t, uint8_t);
void STATE_Set_Day(uint8_t, uint8_t);
void STATE_Set_Hour(uint8_t, uint8_t);
void STATE_Set_Minute(uint8_t, uint8_t);
void STATE_Calibrate_SO2(uint8_t);
void STATE_Calibrate_SO2_ZERO(uint8_t, uint8_t);
void STATE_Calibrate_SO2_SPAN(uint8_t, uint8_t);
void STATE_Calibrate_CO(uint8_t);
void STATE_Calibrate_CO_ZERO(uint8_t, uint8_t);
void STATE_Calibrate_CO_SPAN(uint8_t, uint8_t);
void STATE_Calibrate_EX(uint8_t);
void STATE_Calibrate_EX_ZERO(uint8_t, uint8_t);
void STATE_Calibrate_EX_SPAN(uint8_t, uint8_t);
void STATE_Calibrate_Temp(uint8_t);
void STATE_Calibrate_Temp_SPAN(uint8_t, uint8_t);
void STATE_Set_Alarms_SO2(uint8_t);
void STATE_Set_Alarm_SO2_LO(uint8_t, uint8_t);
void STATE_Set_Alarm_SO2_HI(uint8_t, uint8_t);
void STATE_Set_Alarms_CO(uint8_t);
void STATE_Set_Alarm_CO_LO(uint8_t, uint8_t);
void STATE_Set_Alarm_CO_HI(uint8_t, uint8_t);
void STATE_Set_Alarms_EX(uint8_t);
void STATE_Set_Alarm_EX_LO(uint8_t, uint8_t);
void STATE_Set_Alarm_EX_HI(uint8_t, uint8_t);

//-----PROTOTIPOS DE LA MAQUINA DE ESTADOS PARA DISPARAR LOS 4 CANALES DEL ADC-------

void Start_ADC_Conversions(void);

void TEMP_ADC_Conversion(void);
void SO2_ADC_Conversion(void);
void CO_ADC_Conversion(void);
void EX_ADC_Conversion(void);

//------------------ESTRUCTURAS DE DATOS DE LA UNIDAD--------------------------------
//------Estructuras de sensores------------------------------------------------------

typedef struct SensorData {
    float SO2_Reading;
    float CO_Reading;
    float EX_Reading;
    float TEMP_Reading;
} SensorData_t;


//------Estructuras de sensores------------------------------------------------------

typedef struct TEMP_Parameters{
	float SPAN_Factor;
}TEMP_Parameters_t;

typedef struct SO2_Parameters{
	float ZERO_Factor;
    float SPAN_Factor;
    uint8_t LO_ALarm;
    uint8_t HI_ALarm;
}SO2_Parameters_t;

typedef struct CO_Parameters{
	float ZERO_Factor;
    float SPAN_Factor;
    uint8_t LO_ALarm;
    uint8_t HI_ALarm;
}CO_Parameters_t;

typedef struct EX_Parameters{
	float ZERO_Factor;
    float SPAN_Factor;
    uint8_t LO_ALarm;
    uint8_t HI_ALarm;
}EX_Parameters_t;

//------Estructura de RTC------------------------------------------------------

typedef struct DateTime {
	int32_t Year;
	int32_t Month;
	int32_t Day;
	int32_t Hour;
	int32_t Minute;
	int32_t Second;
} DateTime_t;

//------Estructura de la unidad------------------------------------------------

typedef struct UnitParameters {
    int32_t Serial_Number;
    int32_t ID;
    DateTime_t Data;
    TEMP_Parameters_t TEMP_Config;
    SO2_Parameters_t SO2_Config;
    CO_Parameters_t CO_Config;
    EX_Parameters_t EX_Config;
} UnitParameters_t;       //Parametros que deben almacenarse en memoria no volátil

//-----------------------------------------------------------------------------

#endif /* GASMONITOR_APP_H_ */
