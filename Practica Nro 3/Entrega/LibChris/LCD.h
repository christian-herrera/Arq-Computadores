



#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../Configuraciones.h"


typedef enum {LCD_COMMAND, LCD_DATA} type_data_send;
	



/************************************************************************/
/*                         FUNCIONES PUBLICAS                           */
/************************************************************************/

//	Inicializa el LCD y los valores por defecto
void LCD_Init();


//	Limpia completamente el LCD
void LCD_Clear();


//	Posiciona el cursor en una coordenada (col, row)
//	Parametros:
//	--> [uint8_t] col:  Indice de la columna
//	--> [uint8_t] row:  Indice de la fila
void LCD_setCursor (uint8_t col, uint8_t row);


//	Imprime un caracter en la posicion actual
//	Parametros:
//	--> [uint8_t] data:  Byte que se imprime
void LCD_printChar (uint8_t data);


//	Imprime una cadena de caracteres a partir de la posicion actual
//	Parametros:
//	--> [char *] str:  Inicio de la cadena
void LCD_printString (char *str);


//	Imprime un entero sin signo
//	Parametros:
//	--> [uint16_t] val:  Entero sin signo
void LCD_printInt(uint16_t val);


//	Imprime un Float
//	Parametros:
//	--> [float]		val			: Valor de Punto Flotante
//	--> [int8_t]	min_width	: Tamaño minimo, completa con espacios
//	--> [uint8_t]	dig			: Cantidad de digitos despues del punto decimal
void LCD_printFloat(float val, int8_t min_width, uint8_t dig);


//	Imprime una cadena de caracteres que esta almacenada en la FLASH
//	iniciando en la posicion actual.
//	Parametros:
//	--> [const char *] data:  Direccion de inicio
//
//	Ejemplo:
//	--> const char MSG1[] PROGMEM = "Hola Mundo!";
void LCD_printStringFromFlash(const char * data);






#endif