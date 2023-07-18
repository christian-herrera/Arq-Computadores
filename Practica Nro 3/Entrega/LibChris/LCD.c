

#include "LCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>




/************************************************************************/
/*            DEFINICIONES, VARIABLES Y FUNCIONES PRIVADAS              */
/************************************************************************/
/* Macros que definen los pines como salida */
#define LCD_RS_OUTPUT	DDRD |= (1 << PORTD1)		//RS:	PD1 [1]
#define LCD_E_OUTPUT	DDRD |= (1 << PORTD0)		//E:	PD0 [0]
#define LCD_DB4_OUTPUT	DDRC |= (1 << PORTC1)		//DB4:	PC1 [A1]
#define LCD_DB5_OUTPUT	DDRC |= (1 << PORTC2)		//DB5:	PC2 [A2]
#define LCD_DB6_OUTPUT	DDRB |= (1 << PORTB2)		//DB6:	PB2 [10]
#define LCD_DB7_OUTPUT	DDRB |= (1 << PORTB1)		//DB7:	PB1 [9]

/* Macros que permiten cambiar los estados de los pines */
#define LCD_RS_HIGH PORTD |= (1 << PORTD1)
#define LCD_RS_LOW	PORTD &= ~(1 << PORTD1)
#define LCD_E_HIGH	PORTD |= (1 << PORTD0)
#define LCD_E_LOW	PORTD &= ~(1 << PORTD0)
#define LCD_DB4(data) if(data) PORTC |= (1 << PORTC1); else PORTC &= ~(1 << PORTC1)
#define LCD_DB5(data) if(data) PORTC |= (1 << PORTC2); else PORTC &= ~(1 << PORTC2)
#define LCD_DB6(data) if(data) PORTB |= (1 << PORTB2); else PORTB &= ~(1 << PORTB2)
#define LCD_DB7(data) if(data) PORTB |= (1 << PORTB1); else PORTB &= ~(1 << PORTB1)

/* Mascaras para las diversos modos de funcionamientos */
#define LCD_CMD_CLEAR		(1 << 0)
#define LCD_CMD_HOME		(1 << 1)
#define LCD_CMD_MODE_SET	(1 << 2)
#define LCD_CMD_DISPLAY		(1 << 3)
#define LCD_CMD_CURSOR		(1 << 4)
#define LCD_CMD_FUNCTION	(1 << 5)
#define LCD_CMD_CGRAM_ADD	(1 << 6)
#define LCD_CMD_DDRAM_ADD	(1 << 7)


/* Funciones Privadas */
static void LCD_Send(uint8_t cmd, type_data_send data);
static void LCD_blinkE(uint8_t t1, uint16_t t2);

/* Variables Privadas */
static char LCD_buff[10];




/************************************************************************/
/*                         FUNCIONES PUBLICAS                           */
/************************************************************************/

void LCD_Init(){
	//Pines como salida
	LCD_RS_OUTPUT;
	LCD_E_OUTPUT;
	LCD_DB4_OUTPUT;
	LCD_DB5_OUTPUT;
	LCD_DB6_OUTPUT;
	LCD_DB7_OUTPUT;
	
	_delay_ms(50);		/* LCD Power ON delay always >15ms */
	
	LCD_E_LOW;
	LCD_RS_LOW;
	
	/* Initialization of 16X2 LCD in 4bit mode */
	LCD_DB4(1);
	LCD_DB5(1);
	LCD_DB6(0);
	LCD_DB7(0);
	
	LCD_blinkE(1, 100);
	_delay_us(4500);
	LCD_blinkE(1, 100);
	_delay_us(4500);
	LCD_blinkE(1, 100);
	_delay_us(150);
	
	
	/* 4 Bits + 2 Lineas */
	LCD_Send(LCD_CMD_FUNCTION | 0b00001100, LCD_COMMAND);
	
	/* Encender Pantalla + Cursor Apagado */
	LCD_Send(LCD_CMD_DISPLAY | 0b00000100, LCD_COMMAND);
	
	/* Cursor Incremental */
	LCD_Send(LCD_CMD_MODE_SET | 0b00000010, LCD_COMMAND);
		
	/* Limpiar la pantalla */
	LCD_Clear();
	
	
}




void LCD_Clear(){
	LCD_Send(LCD_CMD_CLEAR, LCD_COMMAND);
	_delay_ms(2);
}




void LCD_setCursor (uint8_t col, uint8_t row){
	LCD_Send(LCD_CMD_DDRAM_ADD | ((col) + 0x40*row), LCD_COMMAND);
}



void LCD_printChar (uint8_t data)	{
	LCD_Send(data, LCD_DATA);
}



void LCD_printString (char *str) {
	while(*str != 0x00){
		LCD_printChar(*str);
		str++;
	}
}


void LCD_printInt(uint16_t val){
	itoa(val, LCD_buff, 10);
	LCD_printString(LCD_buff);
}

void LCD_printFloat(float val, int8_t min_width, uint8_t dig){
	dtostrf(val, min_width, dig, LCD_buff);
	LCD_printString(LCD_buff);
}


/*
 * Ejemplo:  const char MSG1[] PROGMEM = "Hola Mundo!";
 */
void LCD_printStringFromFlash(const char * data){
	for(uint8_t i = 0; pgm_read_byte(data+i) != '\0'; i++){
		LCD_printChar(pgm_read_byte(data+i));
	}
}



/************************************************************************/
/*                         FUNCIONES PRIVADAS                           */
/************************************************************************/

static void LCD_Send(uint8_t cmd, type_data_send data){
	if(data == LCD_COMMAND)
	LCD_RS_LOW;
	else if(data == LCD_DATA)
	LCD_RS_HIGH;
	
	
	//4 Bits mas significativos
	LCD_DB4((cmd >> 4) & 0x01);
	LCD_DB5((cmd >> 5) & 0x01);
	LCD_DB6((cmd >> 6) & 0x01);
	LCD_DB7((cmd >> 7) & 0x01);

	LCD_blinkE(1, 1);
	
	//4 Bits menos significativos
	LCD_DB4(cmd & 0x01);
	LCD_DB5((cmd >> 1) & 0x01);
	LCD_DB6((cmd >> 2) & 0x01);
	LCD_DB7((cmd >> 3) & 0x01);

	LCD_blinkE(1, 100);
}


static void LCD_blinkE(uint8_t t1, uint16_t t2){
	LCD_E_HIGH;		/* Enable pulse */
	for(uint8_t i = 0; i < t1; i++)  _delay_us(1);
	LCD_E_LOW;
	for(uint8_t i = 0; i < t2; i++)  _delay_us(1);
}