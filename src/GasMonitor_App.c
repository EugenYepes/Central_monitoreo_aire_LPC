/*
 * GasMonitor_App.c
 *
 *  Created on: 5 nov. 2022
 *      Author: Pablo Victoria Koruza
 */

#include <Defines.h>

SensorData_t Data_NoFiltered;        //Variables de filtrado de ADC
SensorData_t Data_FilterBuffer[NUMBER_OF_ADC_SAMPLES];
SensorData_t Data_Filtered;
SensorData_t Data_Filtered_Corrected;
extern AirData_t data;

uint32_t FLAG_TEMP_FILTER = 0;      //Flags para la función de filtrado de ADC
uint32_t FLAG_SO2_FILTER = 0;
uint32_t FLAG_CO_FILTER = 0;
uint32_t FLAG_EX_FILTER = 0;

UnitParameters_t Unit_Data;        //Contiene toda la configuración básica del equipo
UnitParameters_t Unit_Data_NEW;    //Contiene la configuración provisoria al momento del seteo

states_t STATE = 0;
uint8_t STATE_ADC = 0;

uint8_t mem = 0;

//---------------FLAGS: POR SER GLOBALES SE DECLARAN 8 BITS PARA AHORRAR MEMORIA ESTATICA----------------
uint8_t ExternalClock = DISABLED;
uint8_t MeasMode = DISABLED;
uint8_t AlarmsMode = DISABLED;
uint8_t ReceiveDataMode = DISABLED;
uint8_t dataParsed = 0; // 0 cuando cuando es necesario parsear los datos, 1 cuando ya se parseo
//-------------------------------------------------------------------------------------------------------

uint8_t LCD_Buffer_Line1[20];
uint8_t LCD_Buffer_Line2[20];



void Alarms_Sentinel(void)
{
	if( (ENABLED == AlarmsMode) && ((Data_Filtered.SO2_Reading >= Unit_Data.SO2_Config.LO_ALarm) || (Data_Filtered.CO_Reading >= Unit_Data.CO_Config.LO_ALarm) || (Data_Filtered.EX_Reading >= Unit_Data.EX_Config.LO_ALarm)) )
	{
		EXTERNALMODULES_Lo_Alarm(ON);

		if((Data_Filtered.SO2_Reading >= Unit_Data.SO2_Config.LO_ALarm) && (Data_Filtered.CO_Reading < Unit_Data.CO_Config.LO_ALarm) && (Data_Filtered.EX_Reading < Unit_Data.EX_Config.LO_ALarm))
		{
			STATE = Show_SO2;   //Si la alarma se activa por el SO2, pasa al estado Show_SO2 y lo muestra en pantalla
		}

		if((Data_Filtered.SO2_Reading < Unit_Data.SO2_Config.LO_ALarm) && (Data_Filtered.CO_Reading >= Unit_Data.CO_Config.LO_ALarm) && (Data_Filtered.EX_Reading < Unit_Data.EX_Config.LO_ALarm))
		{
			STATE = Show_CO;    //Si la alarma se activa por el CO, pasa al estado Show_CO y lo muestra en pantalla
		}

		if((Data_Filtered.SO2_Reading < Unit_Data.SO2_Config.LO_ALarm) && (Data_Filtered.CO_Reading < Unit_Data.CO_Config.LO_ALarm) && (Data_Filtered.EX_Reading >= Unit_Data.EX_Config.LO_ALarm))
		{
			STATE = Show_EX;    //Si la alarma se activa por el EX, pasa al estado Show_EX y lo muestra en pantalla
		}

		if((Data_Filtered.SO2_Reading >= Unit_Data.SO2_Config.HI_ALarm) || (Data_Filtered.CO_Reading >= Unit_Data.CO_Config.HI_ALarm) || (Data_Filtered.EX_Reading >= Unit_Data.EX_Config.HI_ALarm))
		{
			EXTERNALMODULES_Hi_Alarm(ON);
		}
		else
		{
			EXTERNALMODULES_Hi_Alarm(OFF);
		}
	}
	else
	{
		EXTERNALMODULES_Lo_Alarm(OFF);
		EXTERNALMODULES_Hi_Alarm(OFF);
	}
}

void Sensor_DataCorrecter(uint32_t sensor)  //Corrige las lecturas de sensores con los factores de calibracion ZERO y SPAN
{
	if(TEMP_ADC_Channel == sensor)
	{
		Data_Filtered_Corrected.TEMP_Reading = Unit_Data.TEMP_Config.SPAN_Factor * Data_Filtered.TEMP_Reading;
	}
	if(SO2_ADC_Channel == sensor)
	{
		Data_Filtered_Corrected.SO2_Reading = (Unit_Data.SO2_Config.SPAN_Factor * Data_Filtered.SO2_Reading) + Unit_Data.SO2_Config.ZERO_Factor;
	}
	if(CO_ADC_Channel == sensor)
	{
		Data_Filtered_Corrected.CO_Reading = (Unit_Data.CO_Config.SPAN_Factor * Data_Filtered.CO_Reading) + Unit_Data.CO_Config.ZERO_Factor;
	}
	if(EX_ADC_Channel == sensor)
	{
		Data_Filtered_Corrected.EX_Reading = (Unit_Data.EX_Config.SPAN_Factor * Data_Filtered.EX_Reading) + Unit_Data.EX_Config.ZERO_Factor;
	}
}

void Sensor_DataFilter(uint32_t sensor) // IMPORTANTE Esta funcion filtra los ADC, corrige con Sensor_DataCorrecter() y finalmente carga la estructura de mediciones para el módulo TLV
{
	uint32_t i;

	if(TEMP_ADC_Channel == sensor)
	{
		if(FLAG_TEMP_FILTER < NUMBER_OF_ADC_SAMPLES)
		{
			Data_FilterBuffer[FLAG_TEMP_FILTER].TEMP_Reading = Data_NoFiltered.TEMP_Reading;
			FLAG_TEMP_FILTER++;
		}
		else
		{
			FLAG_TEMP_FILTER = 0;
			for(i = 0 ; i < NUMBER_OF_ADC_SAMPLES ; i++)
			{
				Data_Filtered.TEMP_Reading += Data_FilterBuffer[i].TEMP_Reading;
			}

			Data_Filtered.TEMP_Reading = Data_Filtered.TEMP_Reading / NUMBER_OF_ADC_SAMPLES;

			Sensor_DataCorrecter(sensor);     //Llamo a funcion de correccion por factores de calibración

			if(Data_Filtered_Corrected.TEMP_Reading < 0)   //Filtro de negativos al módulo TLV
			{
				data.temperature = 0;
			}
			else
				{
					data.temperature = Data_Filtered_Corrected.TEMP_Reading;          //Saco medicion filtrada y corregida de TEMP al modulo TLV
				}
		}
	}

	if(SO2_ADC_Channel == sensor)
	{
		if(FLAG_SO2_FILTER < NUMBER_OF_ADC_SAMPLES)
		{
			Data_FilterBuffer[FLAG_SO2_FILTER].SO2_Reading = Data_NoFiltered.SO2_Reading;
			FLAG_SO2_FILTER++;
		}
		else
		{
			FLAG_SO2_FILTER = 0;
			for(i = 0 ; i < NUMBER_OF_ADC_SAMPLES ; i++)
			{
				Data_Filtered.SO2_Reading += Data_FilterBuffer[i].SO2_Reading;
			}
			Data_Filtered.SO2_Reading = Data_Filtered.SO2_Reading / NUMBER_OF_ADC_SAMPLES;

			Sensor_DataCorrecter(sensor);     								//Llamo a funcion de correccion por factores de calibración

			if(Data_Filtered_Corrected.SO2_Reading < 0)   //Filtro de negativos al módulo TLV
			{
				data.sulfurDioxide = 0;
			}
			else
				{
					data.sulfurDioxide = Data_Filtered_Corrected.SO2_Reading;          //Saco medicion filtrada y corregida de SO2 al modulo TLV
				}
		}
	}

	if(CO_ADC_Channel == sensor)
	{
		if(FLAG_CO_FILTER < NUMBER_OF_ADC_SAMPLES)
		{
			Data_FilterBuffer[FLAG_CO_FILTER].CO_Reading = Data_NoFiltered.CO_Reading;
			FLAG_CO_FILTER++;
		}
		else
		{
			FLAG_CO_FILTER = 0;
			for(i = 0 ; i < NUMBER_OF_ADC_SAMPLES ; i++)
			{
				Data_Filtered.CO_Reading += Data_FilterBuffer[i].CO_Reading;
			}
			Data_Filtered.CO_Reading = Data_Filtered.CO_Reading / NUMBER_OF_ADC_SAMPLES;

			Sensor_DataCorrecter(sensor);     								//Llamo a funcion de correccion por factores de calibración

			if(Data_Filtered_Corrected.CO_Reading < 0)   //Filtro de negativos al módulo TLV
			{
				data.carbonMonoxide = 0;
			}
			else
				{
					data.carbonMonoxide = Data_Filtered_Corrected.CO_Reading;          //Saco medicion filtrada y corregida de CO al modulo TLV
				}
		}
	}

	if(EX_ADC_Channel == sensor)
	{
		if(FLAG_EX_FILTER < NUMBER_OF_ADC_SAMPLES)
		{
			Data_FilterBuffer[FLAG_EX_FILTER].EX_Reading = Data_NoFiltered.EX_Reading;
			FLAG_EX_FILTER++;
		}
		else
		{
			FLAG_EX_FILTER = 0;
			for(i = 0 ; i < NUMBER_OF_ADC_SAMPLES ; i++)
			{
				Data_Filtered.EX_Reading += Data_FilterBuffer[i].EX_Reading;
			}
			Data_Filtered.EX_Reading = Data_Filtered.EX_Reading / NUMBER_OF_ADC_SAMPLES;

			Sensor_DataCorrecter(sensor);     								 //Llamo a funcion de correccion por factores de calibración

			if(Data_Filtered_Corrected.EX_Reading < 0)   //Filtro de negativos al módulo TLV
			{
				data.lowerExplosiveLimit = 0;
			}
			else
				{
					data.lowerExplosiveLimit = Data_Filtered_Corrected.EX_Reading;          //Saco medicion filtrada y corregida de EX al modulo TLV
				}

		}
	}
}


void Setup_Default_Parameters(void)      //CARGA PARAMETROS DE FABRICA SI LA MEMORIA EEPROM NO ESTÁ PRESENTE
{
	Unit_Data.Data.Year = 22;     // 01/11/22
	Unit_Data.Data.Month = 11;
	Unit_Data.Data.Day = 1;
	Unit_Data.Data.Hour = 0;
	Unit_Data.Data.Minute = 0;
	Unit_Data.Serial_Number = SERIAL_NUMBER;
	Unit_Data.ID = DEFAULT_ID;
	Unit_Data.TEMP_Config.SPAN_Factor = DEFAULT_TEMP_SPAN_FACTOR;
	Unit_Data.SO2_Config.ZERO_Factor = DEFAULT_SO2_ZERO_FACTOR;
	Unit_Data.SO2_Config.SPAN_Factor = DEFAULT_SO2_SPAN_FACTOR;
	Unit_Data.CO_Config.ZERO_Factor = DEFAULT_CO_ZERO_FACTOR;
	Unit_Data.CO_Config.SPAN_Factor = DEFAULT_CO_SPAN_FACTOR;
	Unit_Data.EX_Config.ZERO_Factor = DEFAULT_EX_ZERO_FACTOR;
	Unit_Data.EX_Config.SPAN_Factor = DEFAULT_EX_SPAN_FACTOR;
	Unit_Data.SO2_Config.LO_ALarm = DEFAULT_SO2_LO_ALARM;
	Unit_Data.SO2_Config.HI_ALarm = DEFAULT_SO2_HI_ALARM;
	Unit_Data.CO_Config.LO_ALarm = DEFAULT_CO_LO_ALARM;
	Unit_Data.CO_Config.HI_ALarm = DEFAULT_CO_HI_ALARM;
	Unit_Data.EX_Config.LO_ALarm = DEFAULT_EX_LO_ALARM;
	Unit_Data.EX_Config.HI_ALarm = DEFAULT_EX_HI_ALARM;
}


/*
void Unit_WakeUp(void)
{
	struct wakeUpMessage {
		uint32_t time;
		char *text;
	};
	struct wakeUpMessage message[] = {{500, ""}, {2000, "Multigas Monitor"}, {3000, "UTN FRBA Info II"}, {3000, "Pablo Victoria K"}, {3000, "Eugenio Yepes"}};
	uint8_t idx;
	STATE = STATE_INIT;
	for (idx = 0; idx < NUM_ELEMENTS(message); idx++)
	{
		MAQTIMER_Set(TIMER_WakeUp, message[idx].time, 0, NULL);
		LCD_Full_Display(message[idx].text, " ");
	}
	// continue with the initialization flow
	MAQTIMER_Set(TIMER_WakeUp, 3000, 0, WakeUp_Step2_Setup);
}
*/

void Unit_WakeUp(void)
{
	STATE = STATE_INIT;     		//Antes que nada me aseguro que esté deshabilitada la máquina de estados del menú contextual
	MAQTIMER_Set(TIMER_WakeUp, 500, 0, WakeUp_Step1_Hello1);     //Espera que enciendan todos los perifericos
}
void WakeUp_Step1_Hello1(void)
{
	LCD_Full_Display("  Industrial", "  Gas Monitor");   //Bienvenida

	MAQTIMER_Set(TIMER_WakeUp, 2000, 0, WakeUp_Step1_Hello2);
}
void WakeUp_Step1_Hello2(void)
{
	LCD_Full_Display("    Made in", "   Argentina");   //Bienvenida

	MAQTIMER_Set(TIMER_WakeUp, 2000, 0, WakeUp_Step1_Hello3);
}
void WakeUp_Step1_Hello3(void)
{
	LCD_Full_Display("   UTN FRBA", "Informatica II");   //Bienvenida

	MAQTIMER_Set(TIMER_WakeUp, 3000, 0, WakeUp_Step1_Hello4);
}
void WakeUp_Step1_Hello4(void)
{
	LCD_Full_Display("Pablo Victoria", "Eugenio Yepes");   //Bienvenida

	MAQTIMER_Set(TIMER_WakeUp, 3000, 0, WakeUp_Step2_Setup);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------IMPORTANTE MOMENTO DE CONFIGURACIÓN DE TODA LA UNIDAD--------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void WakeUp_Step2_Setup(void)
{
	LCD_Full_Display("Setting Up Unit", "Please Wait...");

	EXTERNALMODULES_Lo_Alarm(OFF);												//Me aseguro de que la alarma baja este apagada

	EXTERNALMODULES_Hi_Alarm(OFF);												//Me aseguro de que la alarma alta este apagada

	Setup_Default_Parameters();         			                            //CARGA PARAMETROS DE FABRICA SI LA MEMORIA EEPROM NO ESTÁ PRESENTE

	Start_ADC_Conversions();						                            //Dispara la maquina de conversión de ADC para los sensores

	//MAQTIMER_Set (TIMER_TLV_Module_Manager,1000 ,1000 ,TLV_Module_Manager);   //NO LO USO CON MAQ DE TIMERS PORQUE ES MUY PESADO Y FLASHEA EL LCD. LO EJECUTO EN EL SYSTICK

	Unit_Data_NEW = Unit_Data;   // I M P O R T A N T E: Carga la estructura de datos de configuración provisoria

	MAQTIMER_Set(TIMER_Alarms_Sentinel, 200, 200, Alarms_Sentinel);             //Configuro la funcion de chequeo y disparo de alarmas cada 200 mS

	MAQTIMER_Set(TIMER_WakeUp, 3000, 0, WakeUp_Step3_SN);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void WakeUp_Step3_SN(void)
{
	sprintf(LCD_Buffer_Line1,"S/N: %d", Unit_Data.Serial_Number);
	sprintf(LCD_Buffer_Line2,"ID: %03d", Unit_Data.ID);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);
	MAQTIMER_Set(TIMER_WakeUp, 2000, 0, WakeUp_Step4_GasList);
}
void WakeUp_Step4_GasList(void)
{
	LCD_Full_Display("Gases:", "SO2 + CO + Ex");
	MAQTIMER_Set(TIMER_WakeUp, 2000, 0, WakeUp_Step5_WarmUp);
}
void WakeUp_Step5_WarmUp(void)
{
	LCD_Full_Display("Warm up sensors", "Please wait...");
	MAQTIMER_Set(TIMER_WakeUp, 10000, 0, WakeUp_Step6_StartUnit);
}
void WakeUp_Step6_StartUnit(void)
{
	MeasMode = ENABLED;              //Configura modo de medición para MeasModeEN_AlarmsEN
	AlarmsMode = ENABLED;
	ExternalClock = ENABLED;

	LCD_Full_Display("Meas Mode:EN","Alarms:EN");

	STATE = MeasModeEN_AlarmsEN;			 //Habilita la máquina de estados del menú contextual
}


void MeasMode_Sentinel(void)
{
	if(ENABLED == MeasMode)
	{
		EXTERNALMODULES_Status_Leds(ONLINE);
	}
	if(DISABLED == MeasMode)
	{
		EXTERNALMODULES_Status_Leds(OFFLINE);
	}
}

void TLV_Module_Manager(void)
{
	if(ENABLED == MeasMode)
	{
		sendTLVtoUART();
	}
	if(ENABLED == ReceiveDataMode)
	{
		sendTLVtoUART_request();
	}
}

void Start_ADC_Conversions(void)
{
	MAQTIMER_Set(TIMER_Start_ADC_Conversions, ADC_SAMPLING_PERIOD, 0, TEMP_ADC_Conversion);
}
void TEMP_ADC_Conversion(void)
{
	ADC_Disparo(TEMP_ADC_Channel);
	MAQTIMER_Set(TIMER_SO2_ADC_Conversion, ADC_SAMPLING_PERIOD, 0, SO2_ADC_Conversion);
}
void SO2_ADC_Conversion(void)
{
	ADC_Disparo(SO2_ADC_Channel);
	MAQTIMER_Set(TIMER_CO_ADC_Conversion, ADC_SAMPLING_PERIOD, 0, CO_ADC_Conversion);
}
void CO_ADC_Conversion(void)
{
	ADC_Disparo(CO_ADC_Channel);
	MAQTIMER_Set(TIMER_EX_ADC_Conversion, ADC_SAMPLING_PERIOD, 0, EX_ADC_Conversion);
}
void EX_ADC_Conversion(void)
{
	ADC_Disparo(EX_ADC_Channel);
	MAQTIMER_Set(TIMER_TEMP_ADC_Conversion, 50, 0, TEMP_ADC_Conversion);
}

void STATEMACHINE_ContextMenu(void)
{
	uint8_t Tecla = getKey();
	uint8_t Tecla_Multi = getKey_Multi();

	switch(STATE)
	{
		case 0:                     //NO HACE NADA HASTA QUE TERMINE EL MÓDULO WAKEUP (MOTOR DE INICIALIZACION DE LA APLICACIÓN)
	         break;

		case MeasModeEN_AlarmsEN:
    	     STATE_MeasModeEN_AlarmsEN(Tecla);
    	     break;

    	case MeasModeEN_AlarmsDIS:
    	     STATE_MeasModeEN_AlarmsDIS(Tecla);
    	     break;

    	case MeasModeDIS_AlarmsDIS_SMODE:
    	     STATE_MeasModeDIS_AlarmsDIS_SMODE(Tecla);
    	     break;

        case Show_DateTime:
    	     STATE_Show_DateTime(Tecla);
    	     break;

    	case Show_IDSN:
    	     STATE_Show_IDSN(Tecla);
    	     break;

    	case Settings:
    	     STATE_Settings(Tecla);
    	     break;

        case Show_SO2:
    	     STATE_Show_SO2(Tecla);
    	     break;

    	case Show_CO:
    	     STATE_Show_CO(Tecla);
    	     break;

    	case Show_EX:
    	     STATE_Show_EX(Tecla);
    	     break;

    	case Show_Temp:
    	     STATE_Show_Temp(Tecla);
    	     break;

        case Show_SO2_Alarms:
    	     STATE_Show_SO2_Alarms(Tecla);
    	     break;

    	case Show_CO_Alarms:
    	     STATE_Show_CO_Alarms(Tecla);
    	     break;

    	case Show_EX_Alarms:
    	     STATE_Show_EX_Alarms(Tecla);
    	     break;


 /////////////////////////////////////////////////////////////////////////////

        case Receive_Data:
    	     STATE_Receive_Data(Tecla);
    	     break;

        case Receive_Data_LINK:
             STATE_Receive_Data_LINK(Tecla);
             break;

    	case Calibration:
    	     STATE_Calibration(Tecla);
    	     break;

    	case AlarmsSettings:
    	     STATE_AlarmsSettings(Tecla);
    	     break;

    	case ID_Settings:
    	     STATE_ID_Settings(Tecla);
    	     break;

        case Set_ID:
    	     STATE_Set_ID(Tecla);
    	     break;


    	case Set_DateTime:
    	     STATE_Set_DateTime(Tecla);
    	     break;

    	case Set_Year:
    	     STATE_Set_Year(Tecla, Tecla_Multi);
    	     break;

    	case Set_Month:
    	     STATE_Set_Month(Tecla, Tecla_Multi);
    	     break;

    	case Set_Day:
    	     STATE_Set_Day(Tecla, Tecla_Multi);
    	     break;

    	case Set_Hour:
    	     STATE_Set_Hour(Tecla, Tecla_Multi);
    	     break;

        case Set_Minute:
    	     STATE_Set_Minute(Tecla, Tecla_Multi);
    	     break;

    	case Calibrate_SO2:
    	     STATE_Calibrate_SO2(Tecla);
    	     break;

    	case Calibrate_SO2_ZERO:
    	     STATE_Calibrate_SO2_ZERO(Tecla, Tecla_Multi);
    	     break;
    	 case Calibrate_SO2_SPAN:
    	     STATE_Calibrate_SO2_SPAN(Tecla, Tecla_Multi);
    	     break;


    	case Calibrate_CO:
    	     STATE_Calibrate_CO(Tecla);
    	     break;

    	case Calibrate_CO_ZERO:
    	     STATE_Calibrate_CO_ZERO(Tecla, Tecla_Multi);
    	     break;

    	case Calibrate_CO_SPAN:
    	     STATE_Calibrate_CO_SPAN(Tecla, Tecla_Multi);
    	     break;


    	case Calibrate_EX:
    	     STATE_Calibrate_EX(Tecla);
    	     break;

      	case Calibrate_EX_ZERO:
    	     STATE_Calibrate_EX_ZERO(Tecla, Tecla_Multi);
    	     break;

    	case Calibrate_EX_SPAN:
    	     STATE_Calibrate_EX_SPAN(Tecla, Tecla_Multi);
    	     break;

    	case Calibrate_Temp:
    	     STATE_Calibrate_Temp(Tecla);
    	     break;

    	case Calibrate_Temp_SPAN:
    	     STATE_Calibrate_Temp_SPAN(Tecla, Tecla_Multi);
    	     break;

        case Set_Alarms_SO2:
    	     STATE_Set_Alarms_SO2(Tecla);
    	     break;

        case Set_Alarm_SO2_LO:
    	     STATE_Set_Alarm_SO2_LO(Tecla, Tecla_Multi);
    	     break;

        case Set_Alarm_SO2_HI:
    	     STATE_Set_Alarm_SO2_HI(Tecla, Tecla_Multi);
    	     break;

    	case Set_Alarms_CO:
    	     STATE_Set_Alarms_CO(Tecla);
    	     break;

        case Set_Alarm_CO_LO:
    	     STATE_Set_Alarm_CO_LO(Tecla, Tecla_Multi);
    	     break;

        case Set_Alarm_CO_HI:
    	     STATE_Set_Alarm_CO_HI(Tecla, Tecla_Multi);
    	     break;

    	case Set_Alarms_EX:
    	     STATE_Set_Alarms_EX(Tecla);
    	     break;

        case Set_Alarm_EX_LO:
    	     STATE_Set_Alarm_EX_LO(Tecla, Tecla_Multi);
    	     break;

        case Set_Alarm_EX_HI:
    	     STATE_Set_Alarm_EX_HI(Tecla, Tecla_Multi);
    	     break;

    	default:
    		 Unit_WakeUp();    //Si algo anda mal reinicia la aplicación
             break;
	}
}

void STATE_MeasModeEN_AlarmsEN(uint8_t Tecla)      // ESTADO PRIMARIO LUEGO DEL WAKE UP
{
	if(Tecla == TECLA_UP)
	{
		STATE = Show_DateTime;
	}
	if(Tecla == TECLA_EDIT)
	{
		AlarmsMode = DISABLED;

		LCD_Full_Display("Meas Mode:EN","Alarms:DIS");
		STATE = MeasModeEN_AlarmsDIS;
	}
}
void STATE_MeasModeEN_AlarmsDIS(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		STATE = Show_DateTime;
	}

	if(Tecla == TECLA_EDIT)
	{
		MeasMode = DISABLED;
		LCD_Full_Display("Meas Mode:DIS","Alarms:DIS");

		STATE = MeasModeDIS_AlarmsDIS_SMODE;
	}
}
void STATE_MeasModeDIS_AlarmsDIS_SMODE(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		STATE = Show_DateTime;
	}
	if(Tecla == TECLA_EDIT)
	{
		MeasMode = ENABLED;
		AlarmsMode = ENABLED;

		LCD_Full_Display("Meas Mode:EN","Alarms:EN");
		STATE = MeasModeEN_AlarmsEN;
	}
}
void STATE_Show_DateTime(uint8_t Tecla)
{

	sprintf(LCD_Buffer_Line1,"Date: %02d/%02d/%02d",Unit_Data.Data.Day, Unit_Data.Data.Month, Unit_Data.Data.Year, Unit_Data.Data.Hour, Unit_Data.Data.Minute);
	sprintf(LCD_Buffer_Line2,"Time: %02d:%02d:%02d", Unit_Data.Data.Hour, Unit_Data.Data.Minute, Unit_Data.Data.Second);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla == TECLA_UP)
	{
		sprintf(LCD_Buffer_Line1,"SN: %d", Unit_Data.Serial_Number);
		sprintf(LCD_Buffer_Line2,"ID: %03d", Unit_Data.ID);
		LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);
		STATE = Show_IDSN;
	}
}
void STATE_Show_IDSN(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Settings:", " ");

		STATE = Settings;
	}
}
void STATE_Show_SO2(uint8_t Tecla)
{

	if( Data_Filtered_Corrected.SO2_Reading < 0 )   //Filtro de negativos para las lecturas en el LCD
	{
		sprintf(LCD_Buffer_Line1,"SO2: %.1f PPM", 0 * Data_Filtered_Corrected.SO2_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}
	else
	{
		sprintf(LCD_Buffer_Line1,"SO2: %.1f PPM", Data_Filtered_Corrected.SO2_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}

	if(Tecla == TECLA_UP)
	{
		STATE = Show_CO;
	}
}
void STATE_Show_CO(uint8_t Tecla)
{
	if( Data_Filtered_Corrected.CO_Reading < 0 )   //Filtro de negativos para las lecturas en el LCD
	{
		sprintf(LCD_Buffer_Line1,"CO: %.1f PPM", 0 * Data_Filtered_Corrected.CO_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}
	else
	{
		sprintf(LCD_Buffer_Line1,"CO: %.1f PPM", Data_Filtered_Corrected.CO_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}

	if(Tecla == TECLA_UP)
	{
		STATE = Show_EX;
	}
}
void STATE_Show_EX(uint8_t Tecla)
{
	if( Data_Filtered_Corrected.EX_Reading < 0 )   //Filtro de negativos para las lecturas en el LCD
	{
		sprintf(LCD_Buffer_Line1,"Ex: %.1f %%LEL", 0 * Data_Filtered_Corrected.EX_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}
	else
	{
		sprintf(LCD_Buffer_Line1,"Ex: %.1f %%LEL", Data_Filtered_Corrected.EX_Reading);
		LCD_Full_Display(LCD_Buffer_Line1, " ");
	}
	if(Tecla == TECLA_UP)
	{
		sprintf(LCD_Buffer_Line2,"LO:%01d  HI:%01d", Unit_Data.SO2_Config.LO_ALarm, Unit_Data.SO2_Config.HI_ALarm);
		LCD_Full_Display("SO2 Alarms[PPM]", LCD_Buffer_Line2);
		STATE = Show_SO2_Alarms;
	}

}
void STATE_Show_Temp(uint8_t Tecla)
{
	sprintf(LCD_Buffer_Line1,"Temp: %.1f oC", Data_Filtered_Corrected.TEMP_Reading);
	LCD_Full_Display(LCD_Buffer_Line1," ");

	if(Tecla == TECLA_UP)
	{
		STATE = Show_SO2;
	}
}

void STATE_Show_SO2_Alarms(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		sprintf(LCD_Buffer_Line2,"LO:%02d  HI:%02d", Unit_Data.CO_Config.LO_ALarm, Unit_Data.CO_Config.HI_ALarm);
		LCD_Full_Display("CO Alarms[PPM]", LCD_Buffer_Line2);
		STATE = Show_CO_Alarms;
	}
}

void STATE_Show_CO_Alarms(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		sprintf(LCD_Buffer_Line2,"LO:%02d  HI:%02d", Unit_Data.EX_Config.LO_ALarm, Unit_Data.EX_Config.HI_ALarm);
		LCD_Full_Display("Ex Alarms[%LEL]", LCD_Buffer_Line2);
		STATE = Show_EX_Alarms;
	}
}

void STATE_Show_EX_Alarms(uint8_t Tecla)
{

	if(Tecla == TECLA_UP)
	{
		if( ENABLED == MeasMode && ENABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:EN","Alarms:EN");
				STATE = MeasModeEN_AlarmsEN;
			}
		if( ENABLED == MeasMode && DISABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:EN","Alarms:DIS");
				STATE = MeasModeEN_AlarmsDIS;
			}
		if( DISABLED == MeasMode && DISABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:DIS","Alarms:DIS");
				STATE = MeasModeDIS_AlarmsDIS_SMODE;
			}
	}
}


void STATE_Settings(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		STATE = Show_Temp;
	}
	if(Tecla == TECLA_EDIT)
	{
		 if(DISABLED == MeasMode && DISABLED == AlarmsMode)  //Condicion previa para entrar en Calibración
		{
			 LCD_Full_Display("Setup unit", "from PC:");
			 STATE = Receive_Data;
		}
		 if(ENABLED == MeasMode && DISABLED == AlarmsMode)   //Condicion previa para entrar en Config Alarmas
		 {
			 LCD_Full_Display("Alarms Settings:", " ");
			 STATE = AlarmsSettings;
		 }
		 if(ENABLED == MeasMode && ENABLED == AlarmsMode)	//Lo único que se admite en condición MeasMode:EN y AlarmsMode:EN
		 {
			 LCD_Full_Display("Date & Time Set:"," ");
			 STATE = Set_DateTime;
		 }
	}
}

void STATE_Receive_Data(uint8_t Tecla)                          //Receive Data:
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Calibration:", " ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_EDIT)
	{
		ReceiveDataMode = ENABLED;
		ExternalClock = DISABLED;
		LCD_Full_Display("Waiting Data...", "UP to Cancel");
		STATE = Receive_Data_LINK;
	}
}


void STATE_Receive_Data_LINK(uint8_t Tecla)                     //Receive Data:
{
	if(0 == dataParsed)
		Rx_TLVParser();

	if(1 == dataParsed)
	{
		LCD_Full_Display("Config Success!", "UP to Exit");
	}

	if(Tecla == TECLA_UP)
	{
		 dataParsed = 0;
		 ReceiveDataMode = DISABLED;
		 ExternalClock = ENABLED;
		 LCD_Full_Display("Send Config", "from PC:");
		 STATE = Receive_Data;
	}
}

void STATE_Calibration(uint8_t Tecla)                          //Calibration:
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Alarms Settings:", " ");
		STATE = AlarmsSettings;
	}
	if(Tecla == TECLA_EDIT)
	{
		LCD_Full_Display("Calibrate Temp:", " ");
		STATE = Calibrate_Temp;
	}
}

void STATE_Calibrate_Temp(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Calibrate SO2:", " ");
		STATE = Calibrate_SO2;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Calibrate_Temp_SPAN;
	}
}

void STATE_Calibrate_Temp_SPAN(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"Temp: %.1f oC", Data_Filtered_Corrected.TEMP_Reading);
	sprintf(LCD_Buffer_Line2,"CalFc: %.1f", Unit_Data.TEMP_Config.SPAN_Factor);
	LCD_Full_Display(LCD_Buffer_Line1,LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.TEMP_Config.SPAN_Factor < 1.99) Unit_Data.TEMP_Config.SPAN_Factor += 0.005;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.TEMP_Config.SPAN_Factor = 0;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.TEMP_Config.SPAN_Factor = Unit_Data.TEMP_Config.SPAN_Factor; //Refresca la estructura NEW con el nuevo valor del factor

		LCD_Full_Display("Calibration:", " ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.TEMP_Config.SPAN_Factor = Unit_Data_NEW.TEMP_Config.SPAN_Factor;  //En este caso uso NEW para recuperar el anterior factor


		LCD_Full_Display("Calibration:", " ");
		STATE = Calibration;
	}
}

void STATE_Calibrate_SO2(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Calibrate CO:"," ");
		STATE = Calibrate_CO;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Calibrate_SO2_ZERO;
	}
}
void STATE_Calibrate_SO2_ZERO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"SO2: %.1f PPM", Data_Filtered_Corrected.SO2_Reading);
	sprintf(LCD_Buffer_Line2,"ZOffset: %.2f", Unit_Data.SO2_Config.ZERO_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.SO2_Config.ZERO_Factor < 2.99) Unit_Data.SO2_Config.ZERO_Factor += 0.05;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.SO2_Config.ZERO_Factor = -2.99;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.SO2_Config.ZERO_Factor = Unit_Data.SO2_Config.ZERO_Factor; //Refresca la estructura NEW con el nuevo valor del factor
		STATE = Calibrate_SO2_SPAN;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.SO2_Config.ZERO_Factor = Unit_Data_NEW.SO2_Config.ZERO_Factor;  //En este caso uso NEW para recuperar el anterior factor
		STATE = Calibrate_SO2_SPAN;
	}
}
void STATE_Calibrate_SO2_SPAN(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"SO2: %.1f PPM", Data_Filtered_Corrected.SO2_Reading);
	sprintf(LCD_Buffer_Line2,"CalFc: %.2f", Unit_Data.SO2_Config.SPAN_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);


	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.SO2_Config.SPAN_Factor < 2.99) Unit_Data.SO2_Config.SPAN_Factor += 0.05;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.SO2_Config.SPAN_Factor = 0;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.SO2_Config.SPAN_Factor = Unit_Data.SO2_Config.SPAN_Factor; //Refresca la estructura NEW con el nuevo valor del factor


		LCD_Full_Display("Calibration:", " ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.SO2_Config.SPAN_Factor = Unit_Data_NEW.SO2_Config.SPAN_Factor;  //En este caso uso NEW para recuperar el anterior factor


		LCD_Full_Display("Calibration:", " ");
		STATE = Calibration;
	}
}

void STATE_Calibrate_CO(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Calibrate Ex:", " ");
		STATE = Calibrate_EX;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Calibrate_CO_ZERO;
	}
}
void STATE_Calibrate_CO_ZERO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"CO: %.1f PPM", Data_Filtered_Corrected.CO_Reading);
	sprintf(LCD_Buffer_Line2,"ZOffset: %.2f", Unit_Data.CO_Config.ZERO_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.CO_Config.ZERO_Factor < 3.99) Unit_Data.CO_Config.ZERO_Factor += 0.05;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.CO_Config.ZERO_Factor = -3.99;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.CO_Config.ZERO_Factor = Unit_Data.CO_Config.ZERO_Factor; //Refresca la estructura NEW con el nuevo valor del factor
		STATE = Calibrate_CO_SPAN;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.CO_Config.ZERO_Factor = Unit_Data_NEW.CO_Config.ZERO_Factor;  //En este caso uso NEW para recuperar el anterior factor
		STATE = Calibrate_CO_SPAN;
	}
}
void STATE_Calibrate_CO_SPAN(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"CO: %.1f PPM", Data_Filtered_Corrected.CO_Reading);
	sprintf(LCD_Buffer_Line2,"CalFc: %.2f", Unit_Data.CO_Config.SPAN_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.CO_Config.SPAN_Factor < 3.99) Unit_Data.CO_Config.SPAN_Factor += 0.01;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.CO_Config.SPAN_Factor = 0;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.CO_Config.SPAN_Factor = Unit_Data.CO_Config.SPAN_Factor; //Refresca la estructura NEW con el nuevo valor del factor

		LCD_Full_Display("Calibration:"," ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.CO_Config.SPAN_Factor = Unit_Data_NEW.CO_Config.SPAN_Factor;  //En este caso uso NEW para recuperar el anterior factor

		LCD_Full_Display("Calibration:"," ");
		STATE = Calibration;
	}
}

void STATE_Calibrate_EX(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Calibration:"," ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Calibrate_EX_ZERO;
	}
}
void STATE_Calibrate_EX_ZERO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"EX: %.1f %%LEL", Data_Filtered_Corrected.EX_Reading);
	sprintf(LCD_Buffer_Line2,"ZOffset: %.2f", Unit_Data.EX_Config.ZERO_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.EX_Config.ZERO_Factor < 2.99) Unit_Data.EX_Config.ZERO_Factor += 0.05;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.EX_Config.ZERO_Factor = -2.99;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.EX_Config.ZERO_Factor = Unit_Data.EX_Config.ZERO_Factor; //Refresca la estructura NEW con el nuevo valor del factor
		STATE = Calibrate_EX_SPAN;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.EX_Config.ZERO_Factor = Unit_Data_NEW.EX_Config.ZERO_Factor;  //En este caso uso NEW para recuperar el anterior factor
		STATE = Calibrate_EX_SPAN;
	}
}
void STATE_Calibrate_EX_SPAN(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"EX: %.1f %%LEL", Data_Filtered_Corrected.EX_Reading);
	sprintf(LCD_Buffer_Line2,"CalFc: %.2f", Unit_Data.EX_Config.SPAN_Factor);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data.EX_Config.SPAN_Factor < 2.99) Unit_Data.EX_Config.SPAN_Factor += 0.01;  //Incrementa el dato en variable provisoria en pasos de 0,05
		else Unit_Data.EX_Config.SPAN_Factor = 0;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data_NEW.EX_Config.SPAN_Factor = Unit_Data.EX_Config.SPAN_Factor; //Refresca la estructura NEW con el nuevo valor del factor


		LCD_Full_Display("Calibration:"," ");
		STATE = Calibration;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data.EX_Config.SPAN_Factor = Unit_Data_NEW.EX_Config.SPAN_Factor;  //En este caso uso NEW para recuperar el anterior factor


		LCD_Full_Display("Calibration:"," ");
		STATE = Calibration;
	}
}

void STATE_AlarmsSettings(uint8_t Tecla)                     //Alarms Settings:
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Date & Time Set:"," ");
		STATE = Set_DateTime;
	}
	if(Tecla == TECLA_EDIT)
	{
		mem = IIC_Read_Driver(0xA0, 0);  //Lee el byte de memoria y lo guarda en variable global mem
		LCD_Full_Display("Set Alarms SO2:"," ");
		STATE = Set_Alarms_SO2;
	}
}

void STATE_Set_Alarms_SO2(uint8_t Tecla)
{
	if(Tecla == TECLA_EDIT)
    {
		STATE = Set_Alarm_SO2_LO;
	}
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Set Alarms CO:"," ");
		STATE = Set_Alarms_CO;
	}
}
void STATE_Set_Alarm_SO2_LO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"SO2 LO Alarm: %01d", mem);
	sprintf(LCD_Buffer_Line2,"Set new: %01d", Unit_Data_NEW.SO2_Config.LO_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.SO2_Config.LO_ALarm < 9) Unit_Data_NEW.SO2_Config.LO_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.SO2_Config.LO_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.SO2_Config.LO_ALarm = Unit_Data_NEW.SO2_Config.LO_ALarm;

		IIC_Write_Driver(0xA0, 0, Unit_Data_NEW.SO2_Config.LO_ALarm);  //Prueba de grabado de 1 byte en memoria EEPROM

		STATE = Set_Alarm_SO2_HI;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.SO2_Config.LO_ALarm = Unit_Data.SO2_Config.LO_ALarm;
		STATE = Set_Alarm_SO2_HI;
	}
}
void STATE_Set_Alarm_SO2_HI(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"SO2 HI Alarm: %01d", Unit_Data.SO2_Config.HI_ALarm);
	sprintf(LCD_Buffer_Line2,"Set new: %01d", Unit_Data_NEW.SO2_Config.HI_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.SO2_Config.HI_ALarm < 9) Unit_Data_NEW.SO2_Config.HI_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.SO2_Config.HI_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.SO2_Config.HI_ALarm = Unit_Data_NEW.SO2_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms SO2:"," ");
		STATE = Set_Alarms_SO2;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.SO2_Config.HI_ALarm = Unit_Data.SO2_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms SO2:"," ");
		STATE = Set_Alarms_SO2;
	}
}

void STATE_Set_Alarms_CO(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Set Alarms EX:"," ");
		STATE = Set_Alarms_EX;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Set_Alarm_CO_LO;
	}
}
void STATE_Set_Alarm_CO_LO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"CO LO Alarm: %02d", Unit_Data.CO_Config.LO_ALarm);
	sprintf(LCD_Buffer_Line2,"Set new: %02d", Unit_Data_NEW.CO_Config.LO_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.CO_Config.LO_ALarm < 99) Unit_Data_NEW.CO_Config.LO_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.CO_Config.LO_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.CO_Config.LO_ALarm = Unit_Data_NEW.CO_Config.LO_ALarm;
		STATE = Set_Alarm_CO_HI;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.CO_Config.LO_ALarm = Unit_Data.CO_Config.LO_ALarm;
		STATE = Set_Alarm_CO_HI;
	}
}
void STATE_Set_Alarm_CO_HI(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"CO HI Alarm: %02d", Unit_Data.CO_Config.HI_ALarm);
	sprintf(LCD_Buffer_Line2,"Set new: %02d", Unit_Data_NEW.CO_Config.HI_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.CO_Config.HI_ALarm < 99) Unit_Data_NEW.CO_Config.HI_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.CO_Config.HI_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.CO_Config.HI_ALarm = Unit_Data_NEW.CO_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms CO:"," ");
		STATE = Set_Alarms_CO;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.CO_Config.HI_ALarm = Unit_Data.CO_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms CO:"," ");
		STATE = Set_Alarms_CO;
	}
}

void STATE_Set_Alarms_EX(uint8_t Tecla)
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("Alarms Settings:", " ");
		STATE = AlarmsSettings;
	}
	if(Tecla == TECLA_EDIT)
	{
		STATE = Set_Alarm_EX_LO;
	}
}
void STATE_Set_Alarm_EX_LO(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"EX LO Alarm: %02d", Unit_Data.EX_Config.LO_ALarm);
	sprintf(LCD_Buffer_Line2,"Set new: %02d", Unit_Data_NEW.EX_Config.LO_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);


	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.EX_Config.LO_ALarm < 99) Unit_Data_NEW.EX_Config.LO_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.EX_Config.LO_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.EX_Config.LO_ALarm = Unit_Data_NEW.EX_Config.LO_ALarm;
		STATE = Set_Alarm_EX_HI;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.EX_Config.LO_ALarm = Unit_Data.EX_Config.LO_ALarm;
		STATE = Set_Alarm_EX_HI;
	}
}
void STATE_Set_Alarm_EX_HI(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"EX HI Alarm: %02d", Unit_Data.EX_Config.HI_ALarm);
	sprintf(LCD_Buffer_Line2,"Set new: %02d", Unit_Data_NEW.EX_Config.HI_ALarm);
	LCD_Full_Display(LCD_Buffer_Line1, LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.EX_Config.HI_ALarm < 99) Unit_Data_NEW.EX_Config.HI_ALarm ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.EX_Config.HI_ALarm = 1;
	}
	if(Tecla == TECLA_EDIT)    // Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.EX_Config.HI_ALarm = Unit_Data_NEW.EX_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms EX:", " ");
		STATE = Set_Alarms_EX;
	}
	if(Tecla == TECLA_NEXT)    // Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.EX_Config.HI_ALarm = Unit_Data.EX_Config.HI_ALarm;
		LCD_Full_Display("Set Alarms EX:", " ");
		STATE = Set_Alarms_EX;
	}
}


void STATE_ID_Settings(uint8_t Tecla)                    //ID Settings:
{
	if(Tecla == TECLA_EDIT)
	{
		STATE = Set_ID;
	}
	if(Tecla == TECLA_UP)
	{
		if( ENABLED == MeasMode && ENABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:EN","Alarms:EN");
				STATE = MeasModeEN_AlarmsEN;
			}
		if( ENABLED == MeasMode && DISABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:EN","Alarms:DIS");
				STATE = MeasModeEN_AlarmsDIS;
			}
		if( DISABLED == MeasMode && DISABLED == AlarmsMode )
			{
			LCD_Full_Display("Meas Mode:DIS","Alarms:DIS");
				STATE = MeasModeDIS_AlarmsDIS_SMODE;
			}
	}
}

void STATE_Set_ID(uint8_t Tecla)
{
	sprintf(LCD_Buffer_Line1,"ID: %03d", Unit_Data.ID);
	sprintf(LCD_Buffer_Line2,"Set new: %03d", Unit_Data_NEW.ID);
	LCD_Full_Display(LCD_Buffer_Line1,LCD_Buffer_Line2);

	if(Tecla == TECLA_UP) { Unit_Data_NEW.ID ++; }  //Incrementa el dato en variable provisoria

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.ID = Unit_Data_NEW.ID;
		LCD_Full_Display("ID Settings:"," ");
		STATE = ID_Settings;
	}

	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{
		Unit_Data_NEW.ID = Unit_Data.ID;
		LCD_Full_Display("ID Settings:"," ");
		STATE = ID_Settings;
	}

}

void STATE_Set_DateTime(uint8_t Tecla)                   //Date & Time Set:
{
	if(Tecla == TECLA_UP)
	{
		LCD_Full_Display("ID Settings:"," ");
		STATE = ID_Settings;
	}
	if(Tecla == TECLA_EDIT)
	{
		ExternalClock = DISABLED;    //Deshabilito el reloj externo para poder configurarlo

		STATE = Set_Year;
	}
}

void STATE_Set_Year(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"Date: %02d/%02d/%d",Unit_Data.Data.Day, Unit_Data.Data.Month, Unit_Data_NEW.Data.Year);
	LCD_Full_Display(LCD_Buffer_Line1," ");

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.Data.Year < 99) Unit_Data_NEW.Data.Year ++;
		else Unit_Data_NEW.Data.Year = 0;
	}  //Incrementa el dato en variable provisoria

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.Data.Year = Unit_Data_NEW.Data.Year;
		STATE = Set_Month;
	}
	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{									//Pasa a MONTH sin modificar YEAR
		Unit_Data_NEW.Data.Year = Unit_Data.Data.Year;
		STATE = Set_Month;
	}
}
void STATE_Set_Month(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line1,"Date: %02d/%02d/%d",Unit_Data.Data.Day, Unit_Data_NEW.Data.Month, Unit_Data.Data.Year);
	LCD_Full_Display(LCD_Buffer_Line1," ");

	if(Tecla_Multi == TECLA_UP )       //Incrementa el dato en variable provisoria
	{
		if(Unit_Data_NEW.Data.Month < 12) Unit_Data_NEW.Data.Month ++;
		else Unit_Data_NEW.Data.Month = 1;
	}

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.Data.Month = Unit_Data_NEW.Data.Month;
		STATE = Set_Day;
	}
	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{									//Pasa a DAY sin modificar MONTH
		Unit_Data_NEW.Data.Month = Unit_Data.Data.Month;
		STATE = Set_Day;
	}
}
void STATE_Set_Day(uint8_t Tecla, uint8_t Tecla_Multi)
{
	uint32_t Leap_Year_Calc;    //Uso para calcular año bisiesto

	sprintf(LCD_Buffer_Line1,"Date: %02d/%02d/%d",Unit_Data_NEW.Data.Day, Unit_Data.Data.Month, Unit_Data.Data.Year);
	LCD_Full_Display(LCD_Buffer_Line1," ");

	if(Tecla_Multi == TECLA_UP )       //Incrementa el dato en variable provisoria
	{
		if((Unit_Data_NEW.Data.Month % 2) == 0)
		{
			if (Unit_Data_NEW.Data.Month < 7)
			{
//-------------------------- COMIENZO DEL MOTOR DE CALCULO DE AÑOS BISIESTOS----------------------------------------------------------------
				if(Unit_Data_NEW.Data.Month == 2)
				{
						Leap_Year_Calc = Unit_Data_NEW.Data.Year % 10;   //Extrae la unidad

						if((Unit_Data_NEW.Data.Year % 2) != 0)     //Descarto años impares
						{
							if(Unit_Data_NEW.Data.Day < 28) Unit_Data_NEW.Data.Day ++;
							else Unit_Data_NEW.Data.Day = 1;
						}
						else
						{

							if( 0 == Leap_Year_Calc || 4 == Leap_Year_Calc || 8 == Leap_Year_Calc)  //Si la unidad es par y 0 || 4 || 8
							{
								Leap_Year_Calc = (Unit_Data_NEW.Data.Year - Leap_Year_Calc) / 10;     //Extrae la decena

								if((Leap_Year_Calc % 2) == 0) //Siendo la unidad 0 || 4 || 8 y la decena par ES BISIESTO
								{
									if(Unit_Data_NEW.Data.Day < 29) Unit_Data_NEW.Data.Day ++;
									else Unit_Data_NEW.Data.Day = 1;
								}
								else
								{
									if(Unit_Data_NEW.Data.Day < 28) Unit_Data_NEW.Data.Day ++;
									else Unit_Data_NEW.Data.Day = 1;
								}
							}

							else    //Si la unidad es par y 2 || 6
							{
								Leap_Year_Calc = (Unit_Data_NEW.Data.Year - Leap_Year_Calc) / 10;     //Extrae la decena

								if((Leap_Year_Calc % 2) != 0) //Siendo la unidad 2 || 6 y la decena impar ES BISIESTO
								{
									if(Unit_Data_NEW.Data.Day < 29) Unit_Data_NEW.Data.Day ++;
									else Unit_Data_NEW.Data.Day = 1;
								}
								else
								{
									if(Unit_Data_NEW.Data.Day < 28) Unit_Data_NEW.Data.Day ++;
									else Unit_Data_NEW.Data.Day = 1;
								}

							}
						}

					}
//-------------------------- FIN DEL MOTOR DE CALCULO DE AÑOS BISIESTOS--------------------------------------------------------------
				else
				{
					if(Unit_Data_NEW.Data.Day < 30) Unit_Data_NEW.Data.Day ++;
					else Unit_Data_NEW.Data.Day = 1;
				}
			}
			else
			{
				if(Unit_Data_NEW.Data.Day < 31) Unit_Data_NEW.Data.Day ++;
				else Unit_Data_NEW.Data.Day = 1;
			}
		}

		if( (Unit_Data_NEW.Data.Month % 2) != 0 )
		{
			if(Unit_Data_NEW.Data.Month <= 7)
			{
				if(Unit_Data_NEW.Data.Day < 31) Unit_Data_NEW.Data.Day ++;
				else Unit_Data_NEW.Data.Day = 1;
			}
			else
			{
				if(Unit_Data_NEW.Data.Day < 30) Unit_Data_NEW.Data.Day ++;
				else Unit_Data_NEW.Data.Day = 1;
			}
		}
	}

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.Data.Day = Unit_Data_NEW.Data.Day;
		STATE = Set_Hour;
	}
	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{									// Pasa a HOUR sin modificar DAY
		Unit_Data_NEW.Data.Day = Unit_Data.Data.Day;
		STATE = Set_Hour;
	}
}
void STATE_Set_Hour(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line2,"Time: %02d:%02d ", Unit_Data_NEW.Data.Hour, Unit_Data.Data.Minute);
	LCD_Full_Display(" ", LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.Data.Hour < 23) Unit_Data_NEW.Data.Hour ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.Data.Hour = 0;
	}

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.Data.Hour = Unit_Data_NEW.Data.Hour;
		STATE = Set_Minute;
	}
	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{									//Pasa a MIN sin modificar HOUR
		Unit_Data_NEW.Data.Hour = Unit_Data.Data.Hour;
		STATE = Set_Minute;
	}
}
void STATE_Set_Minute(uint8_t Tecla, uint8_t Tecla_Multi)
{
	sprintf(LCD_Buffer_Line2,"Time: %02d:%02d ", Unit_Data.Data.Hour, Unit_Data_NEW.Data.Minute);
	LCD_Full_Display(" ", LCD_Buffer_Line2);

	if(Tecla_Multi == TECLA_UP)
	{
		if(Unit_Data_NEW.Data.Minute < 59) Unit_Data_NEW.Data.Minute ++;  //Incrementa el dato en variable provisoria
		else Unit_Data_NEW.Data.Minute = 0;
	}

	if(Tecla == TECLA_EDIT)						// Valida el dato guardándolo en la variable permanente
	{
		Unit_Data.Data.Minute = Unit_Data_NEW.Data.Minute;

		ExternalClock = ENABLED;   //Vuelvo a habilitar el reloj externo

		LCD_Full_Display("Date & Time Set:"," ");
		STATE = Set_DateTime;
	}
	if(Tecla == TECLA_NEXT)		// Invalida el dato, devolviendo el valor anterior a la variable provisoria
	{									//Pasa a MIN sin modificar HOUR
		Unit_Data_NEW.Data.Minute = Unit_Data.Data.Minute;

		ExternalClock = ENABLED;   //Vuelvo a habilitar el reloj externo

		LCD_Full_Display("Date & Time Set:"," ");
		STATE = Set_DateTime;
	}
}
