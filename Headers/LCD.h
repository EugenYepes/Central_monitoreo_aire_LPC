#ifndef LCD_LCD_H_
#define LCD_LCD_H_

//Para configurar el display con tan solo modificar estas macros:

//Funcion SET
#define LCD_SET_DL			LCD_ALTO		//DL: Interface Control Lenght Bit; 0-4bits, 1-8bits.
#define LCD_SET_N			LCD_ALTO 		//N: Display Line Number; 0- 1line, 1- 2lines.
#define LCD_SET_F			LCD_BAJO		//F: Font Type Control Bit; 0- 5x8 dots, 1- 5x11dots.

//Funcion Blink, Display on
#define LCD_DISPLAY_DISPLAY		LCD_ALTO		//D: Display On/Off; 1-DisplayON, 0-DisplayOFF.
#define LCD_DISPLAY_CURSOR		LCD_BAJO		//C: Cursor On/Off; 1-CursorON, 0-CursorOFF.
#define LCD_DISPLAY_BLINK		LCD_BAJO		//B: Cursor Blink On/Off; 1-BlinkON, 0-BlinkOFF.

//Funcion Entry Mode
#define LCD_ENTRY_MODE_I_D		LCD_ALTO		//I/D: Increment or decrement of DDRAM Adress; 0-Decremet, move cursor to left- 1-Increment, move cursor to right
#define LCD_ENTRY_MODE_S		LCD_BAJO		//S: Shift entyre display; 0-Not Shift; 1-Shift display.


//#define		LCD_RW	         //No lo uso porque solamente voy a escribir y no leer
#define		LCD_RS		0,9
#define		LCD_E		0,8

// pins used for LCD display port,pin
#define		LCD_D7		0,0
#define		LCD_D6		0,1
#define		LCD_D5		0,28
#define		LCD_D4		0,29
#define		LCD_D3		0,30
#define		LCD_D2		0,15
#define		LCD_D1		0,6
#define		LCD_D0		0,31


//----------------------------------------Defines
#define DEMORA_RW			50 //100
#define DEMORA_E			5 //10

#define LCD_ALTO		1
#define LCD_BAJO		0

#define CURSOR_IZQUIERDA	0
#define CURSOR_DERECHA		1


//MI LISTA DE COMANDOS: (Son macros que se usan en funciones).
#define LCD_COMMAND_CLEAR_DISPLAY						0x004
#define LCD_COMMAND_RETURN_HOME							0x008 //Vuelve al inicio del dsp
#define LCD_COMMAND_ENTRY_MODE							0x01F //LB escriben en el ENTRY MODE
#define LCD_COMMAND_DISPLAY_ON_CURSOR_BLINK_ON			0x03C //Que encienda el display, que muestre y titile el cursor.
#define LCD_COMMAND_FUNCION_SET							0x0F0 //Comando para enviar y mostrar info en fila y columna de LB (Mirar hoja de datos).
#define LCD_DATA										0x000
#define LCD_COMMAND_ENTRY_MODE_SET						0x0AB // A medida que voy escribiendo, va moviendose el cursor hacia donde quiero.


void Demora (uint32_t demoras); //Demoras
void LCD_PULSO_E (void); //Funcion que genera el pulso de Enable
void LCD_Init_salidas_8bits (void); //Funcion que inicializa todo como salida para 8 bits.
void LCD_init_8bits (void); //Funcion que inicializa el display LCD, con 8 bits.
void LCD_SET_8bits(void); //Funcion para setear que manda 8 bits y se usan las 2 lineas (Es el del TPO, 20x4)
void LCD_SET_DDRAM (uint32_t fila); //Funcion que setea la DDRAM en la fila que quiera.
void LCD_Move_16x2 (uint32_t fila, uint32_t columna); //Funcion que me mueve a donde quiera en el display.
void LCD_Cursor_Move (uint8_t valor, uint8_t posicion); //Funcion que mueve el curso a la posicion que quiera.
void LCD_Write_8bits (int8_t valor, uint8_t tipo); //Funcion que escribe al display
void LCD_Display(int8_t* msg, uint32_t fila, uint32_t columna); //Funcion que envia el mensaje
void LCD_Full_Display(int8_t* msg1, int8_t* msg2);//Funcion que envia el mensaje EN PANTALLA COMPLETA CON CLEAN SCREEN
void MDE_Display (void); //MDE del display

#endif /* LCD_LCD_H_ */
