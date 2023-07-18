/*
 * MCP4725.c
 *
 * Created: 25/6/2023 22:11:31
 *  Author: christian
 */ 



#include "MCP4725.h"
#include <avr/io.h>
#include <util/delay.h>


static int8_t I2C_is_timeout();

#define I2C_START		0x08
#define I2C_REP_START	0x10
#define I2C_LOST		0x38
#define I2C_MT_ADD_ACK	0x18
#define I2C_MT_DATA_ACK	0x28

#define I2C_SEND_START	(TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN))
#define I2C_SEND_STOP	(TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN))
#define I2C_SET_DATA(data)(TWDR = data)
#define I2C_SEND_WRITE	( TWCR = (1 << TWINT) | (1 << TWEN) )

uint8_t status;


/************************************************************************/
/*                         FUNCIONES PUBLICAS                           */
/************************************************************************/

uint8_t DAC_init(){
	DDRC &= ~((1 << PORTC4) | (1 << PORTC5));
	
	TWSR = 0;			//Prescaler = 1
	TWBR = 0x48;		//Frec. SCL = 100kHz

	return(DAC_checkConnection());
}



uint8_t DAC_checkConnection(){
	//Envio START
	I2C_SEND_START;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_START)	return(0);
		
	//Envio la Direccion
	I2C_SET_DATA(DAC_ADDRESS<<1);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_ADD_ACK) return(0);
		
	//Envio STOP
	I2C_SEND_STOP;
	_delay_us(10);
	
	return(1);
}


uint8_t DAC_setVoltaje(uint16_t val){
	//Envio START
	I2C_SEND_START;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_START)	return(0);
	
	//Envio la Direccion
	I2C_SET_DATA(DAC_ADDRESS<<1);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_ADD_ACK) return(0);
	
	//Envio los datos
	I2C_SET_DATA((val >> 8) & 0x0F);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) return(0);
	
	
	I2C_SET_DATA(val & 0xFF);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) return(0);
	
	
	//Envio STOP
	I2C_SEND_STOP;
	_delay_us(10);
	
	return(1);
}




uint8_t DAC_WriteEEPROM(uint16_t val){
	//Envio START
	I2C_SEND_START;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_START)	return(0);
	
	//Envio la Direccion
	I2C_SET_DATA(DAC_ADDRESS<<1);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_ADD_ACK) return(0);
	
	//Envio la orden de escribir en la EEPROM
	I2C_SET_DATA(0x60);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) return(0);
	
	//Valor a guardar: [12-4]
	I2C_SET_DATA((val >> 4) & 0xFF);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) return(0);
	
	//Valor a guardar: [3-0]
	I2C_SET_DATA((val << 4) & 0xF0);
	I2C_SEND_WRITE;
	if(I2C_is_timeout()) return(0);
	if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) return(0);
	
	
	//Envio STOP
	I2C_SEND_STOP;
	_delay_us(10);
	
	return(1);
}







/************************************************************************/
/*                         FUNCIONES PRIVADAS                           */
/************************************************************************/
//	ESPERA MIENTRAS NO SE SUPERA EL TIME
//	Devuelve:
//		-  1:	TIMEOUT
//		-  0:	Flag captado dentro del time
static int8_t I2C_is_timeout(){
	
	//for(uint16_t i = )
	uint16_t count = 0;
	

	while(!(TWCR & (1 << TWINT))){
		count++;
		_delay_us(1);
		
		if(count > I2C_TIMEOUT){
			return(1);
		}
	}
	
	return(0);
}





