/*
 * MCP4725.h
 *
 * Created: 25/6/2023 22:12:06
 *  Author: christian
 */ 


#ifndef MCP4725_H_
#define MCP4725_H_

#include <avr/io.h>
#include "../Configuraciones.h"






//	Inicializa el DAC
//  Retorno:
//	--> [uint8_t] : 0 -> Falla en la conexion
//				  : 1 -> Dispositivo Online
uint8_t DAC_init();


//	Envia solo el Address para corroborar si recibo el ACK
//  Retorno:
//	--> [uint8_t] : 0 -> Falla en la conexion
//				  : 1 -> Dispositivo Online
uint8_t DAC_checkConnection();


//	Envia el valor de tension al DAC (0 ~ 4095)
//	Parametros:
//	-->	[uint16_t]: val -> Valor para el DAC
//  Retorno:
//	--> [uint8_t] : 0 -> Falla en la conexion
//				  : 1 -> Dispositivo Online
uint8_t DAC_setVoltaje(uint16_t val);


//	Envia el valor de tension al DAC para almacenar 
//	en la EEPROM (0 ~ 4095)
//	Parametros:
//	-->	[uint16_t]: val -> Valor para almacenar
//  Retorno:
//	--> [uint8_t] : 0 -> Falla en la conexion
//				  : 1 -> Dispositivo Online
uint8_t DAC_WriteEEPROM(uint16_t val);



#endif /* MCP4725_H_ */