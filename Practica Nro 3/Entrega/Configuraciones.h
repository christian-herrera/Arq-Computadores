/*
 * Configuraciones.h
 *
 * Created: 26/6/2023 01:34:06
 *  Author: christian
 */ 


#ifndef CONFIGURACIONES_H_
#define CONFIGURACIONES_H_

/************************************************************************/
/*						FRECUENCIA DEL UC                               */
/************************************************************************/
#define F_CPU 16000000UL



/************************************************************************/
/*			CONFIGURACIONES PARA LOS MENUS                              */
/************************************************************************/
// Tiempo de Espera al salir de un menu o submenu
#define DELAY_MENU_EXIT()(_delay_ms(500))







/************************************************************************/
/*			CONFIGURACIONES PARA EL HARDWARE                            */
/************************************************************************/
//	Inversion de la logica del Encoder Rotativo
#define INV_LOGICA_ENCODER	0
#define INV_LOGICA_SW		0








/************************************************************************/
/*			CONFIGURACION DEL CONTROL DE CORRIENTE                      */
/************************************************************************/
//	Maxima corriente permitida en el Setpoint
#define MAX_CURRENT					3000						//mA

//	Maxima corriente permitida por culpa del Mosfet+Resistencias
//	Con este valor, salta la alarma de falla por corriente
#define MAX_HARDWARE_CURRENT		4000						//mA


//	Valor de los saltos de corriente permitidos en el menu.
//	Este valor debera ser multiplo de MAX_CURRENT
#define CURRENT_MIN_SALTOS			100							//mA

//	Valor de los saltos de corriente permitidos en la pantalla de run
//	Este valor debera ser multiplo de MAX_CURRENT
#define CURRENT_MIN_SALTOS_RUNNING	10							//mA

//	Resistor donde se tomara la tension
#define RES_SHUNT					1.0							//Ohms

//	Cantidad de lecturas para generar el promedio
#define CURRENT_PROM_CANT			15

//	MAX_ERROR_CURRENT es el Maximo error que se permite, las comprobaciones
//	inician luego de ERROR_CURRENT_INITIAL_TIME segundos, y debera
//	existir el error durante MAX_TIME_ERROR_CURRENT milisegundos.
#define MAX_ERROR_CURRENT			200							//mA
#define MAX_TIME_ERROR_CURRENT		500							//ms
#define ERROR_CURRENT_INITIAL_TIME	5							//s







/************************************************************************/
/*			CONFIGURACION DEL CONTROL DE TEMPERATURA                    */
/************************************************************************/
//	Maxima temperatura permitida, si se supera entra en modo falla
#define MAX_TEMP			40.0						//Grados

//	Cantidad de lecturas para generar el promedio
#define TEMP_PROM_CANT		15

//	Resistencia en serie con el Termistor
#define Rc	100000.0									//Ohms

//	Constantes para obtener la temperatura. Sacados del vendedor
#define To	25.0
#define B	3950.0
#define Ro	100000.0





/************************************************************************/
/*			PROGRAMA - CORRIENTE CONSTANTE                              */
/************************************************************************/
//	Tiempo de espera maximo permitido
#define MAX_TIME_OBJ		60*12						//min [12 hs = 720 min]







/************************************************************************/
/*			DAC	-	CONFIGURACIONES DEL MCP4725                         */
/************************************************************************/
//	Direccion I2C del dispositivo DAC
#define DAC_ADDRESS 0x60

//	Maximo tiempo que espero la respuesta del DAC por I2C
#define I2C_TIMEOUT	5000									//us










#endif /* CONFIGURACIONES_H_ */