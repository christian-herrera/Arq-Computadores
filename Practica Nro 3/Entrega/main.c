
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/atomic.h>
#include "LibChris/LCD.h"
#include "LibChris/MCP4725.h"
#include "Configuraciones.h"



/************************************************************************/
/*								MACROS                                  */
/************************************************************************/
//Control de Corriente
#define CURRENT_ADC_TO_MA(adcval)( adcval *  (5000.0/1023.0)/RES_SHUNT)
#define I_TO_DAC(mA)( (uint16_t)(mA*RES_SHUNT*4095.0/5000.0))
#define DAC_TO_I(adcVal) ( 5000.0*adcVal/(4095.0*RES_SHUNT))

//Control de Temperatura
#define TEMP_ADC_TO_VOLTS(adcVal)( adcVal * (5.0/1023.0) )
#define TEMP_ADC_TO_RNTC(adcVal)( (adcVal*Rc)/(1023 - adcVal) )
#define TEMP_CONVERT_TO_CELCIUS(Radc)( 1/( 1/(To+273.15) + log(Radc/Ro)/B ) - 273.15)

//Encoder Rotativo
#define BUTTON() INV_LOGICA_SW ? (PIND & (1 << PORTD6)) : !(PIND & (1 << PORTD6))




/************************************************************************/
/*							DECLARACIONES                               */
/************************************************************************/
//Estados generales
uint8_t status;


//Implementacion de la funcion millis()
volatile unsigned long millis_counter = 0;
unsigned long millis();


//Maquina de Estados
typedef enum{HOME, SUB_CONST_CURRENT, SUB_RAMP_CURRENT} var_menu;
typedef enum{SELECT_I, SELECT_TIME_MAX, CANCELAR, INICIAR, RUN_I_CONST_A, RUN_I_CONST_B} var_subMenu;
char* submenu_title[] = {"I: Const.", "I: Ramp", "Factory Reset"};
#define SUB_MAX 2
var_menu menu;
var_subMenu subMenu;
int8_t subMenuIndex = 0;

//Funciones que imprimen el diseño especifico en el LCD
void printMenu(uint8_t clean);
void printMsjeFactoryReset();
void printSubMenuIConst(uint8_t index, uint8_t clean);
void printAlertMaxCurrent();
void printAlertMaxTemp();
void printMsjeTiempo();
void printMsjeFin();
void printErrorDAC();
void printMAH();


//Encoder Rotativo
int8_t readEncoder();
uint8_t readButton();


//Control de Corriente
uint16_t I_obj = 0;
uint16_t t_obj = 0;
uint16_t t_acum = 0;
unsigned long mark_t_zero;
unsigned long mark_dt_mah;



//Lectura de los ADC
float I_read;
float T_read;
float I_mAh;
volatile uint16_t I_read_raw = 0;
volatile uint16_t I_read_raw_acum = 0;
volatile uint8_t I_read_prom_count = CURRENT_PROM_CANT;
volatile uint16_t T_read_raw = 0;
volatile uint16_t T_read_raw_acum = 0;
volatile uint8_t T_read_prom_count = TEMP_PROM_CANT;
void ADC_readInit();
void ADC_readCurrent();
void ADC_readTemperature();
void printMsjeCurrentFail();




/************************************************************************/
/*						MAIN PRINCIPAL                                  */
/************************************************************************/
int main(void) {
	
	status = DAC_init();
	if(status == 1)
		DAC_setVoltaje(0);
	
	
	//Configuracion del Timer 1A (16 bits) para implementar millis()
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);		// Timer 1 en modo CTC, y prescaler de 64 (F_CPU/64)
	OCR1A = 249;											// OCR1A en 249 para obtener 1ms
	TIMSK1 |= (1 << OCIE1A);								// Interrupcion por comparacion
	
	
	//ADC
	DDRC &= ~((1 << PORTC3) | (1 << PORTC0));
	ADMUX  = (1 << REFS0);									//Referencia: AVcc
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	//125kHz
	ADCSRA |= (1 << ADIE) | (1 << ADEN);					//Interrupciones | Habilito el ADC
	ADC_readInit();
	
	
	
	//Rotary Encoder
	DDRD &= ~((1 << PORTD7) | (1 << PORTD6));				//Como entradas y con Pull-Up
	DDRB &= ~(1 << PORTB0);
	PORTD |= (1 << PORTD7) | (1 << PORTD6);
	PORTB |= (1 << PORTB0);
	
	
	
	//Pantalla LCD
	LCD_Init();
	
	if(status == 1){
		LCD_setCursor(6, 0);
		LCD_printString("CARGA");
		LCD_setCursor(3, 1);
		LCD_printString("ELECTRONICA");
		_delay_ms(2000);
		LCD_Clear();
		LCD_printString("MENU PRINCIPAL");
		LCD_setCursor(0, 1);
		LCD_printString(submenu_title[0]);
		LCD_setCursor(15, 1);
		LCD_printString(">");
	} else {
		printErrorDAC();		
	}
	
	
	
	
	
	//Estados iniciales
	menu = HOME;
	subMenu = SELECT_I;
	int8_t temp = 0;
	subMenuIndex = 0;


	sei();
	
	
    /************************************************************************/
    /*							LOOP PRINCIPAL                              */
    /************************************************************************/
    while (1) {
		temp = readEncoder();
		
		switch(menu){
			/************************************************************************/
			/*						MENU PRINCIPAL -> HOME                          */
			/* En esta seccion, me puedo mover entre los nombres de los Submenus	*/
			/* disponibles mediante el giro del encoder. Para entrar, simplemente	*/
			/* presiono el encoder.													*/
			/************************************************************************/
			case HOME:
			//Acciones al girar
			if(temp != 0){
				subMenuIndex += temp;										//Muevo el indice sin irme del rango
				if(subMenuIndex > SUB_MAX) subMenuIndex = SUB_MAX;
				if(subMenuIndex < 0) subMenuIndex = 0;
				
				printMenu(0);
			}
			
			//Acciones al apretar el boton
			if(readButton()){
				switch(subMenuIndex){
					case 0:													//Menu: I. Constante -> Voy al submenu I-Cte
					menu = SUB_CONST_CURRENT;
					subMenu = SELECT_I;
					I_obj = CURRENT_MIN_SALTOS;
					t_obj = 0;
					printSubMenuIConst(10, 1);
					break;
					
					case 1:													//Menu: I. Rampa	-> No implementado todavia
					//menu = SUB_RAMP_CURRENT;
					//subMenu = SELECT_I;
					//printSubMenuIRamp(20, 1);
					break;
					
					case 2:													//Menu: Factory Reset	-> Realizo el guardado en
					if(!DAC_WriteEEPROM(0)){								//						   la EEPROM del DAC y espero el reset
						printErrorDAC();
					} else {
						printMsjeFactoryReset();
					}
					break;
				}
				
			}
			break;
			
			/************************************************************************/
			/*					SUBMENU -> CORRIENTE CONSTANTE						*/
			/* En esta seccion, me muevo dentro del Submenu: Corriente Constante	*/
			/* Desde aca se configura la corriente deseada, el tiempo maximo y se	*/
			/* inicializa.															*/
			/************************************************************************/
			case SUB_CONST_CURRENT:
						
			//Acciones al Girar
			if(temp != 0){
				switch(subMenu){
					case SELECT_I:																			//Seleccion de I
					if(I_obj == CURRENT_MIN_SALTOS && temp == 1){
						I_obj += temp * CURRENT_MIN_SALTOS;
					} else if(I_obj > CURRENT_MIN_SALTOS && I_obj < MAX_CURRENT){
						I_obj += temp * CURRENT_MIN_SALTOS;
					} else if(I_obj == MAX_CURRENT && temp == -1){
						I_obj += temp * CURRENT_MIN_SALTOS;
					}
					
					printSubMenuIConst(10, 0);
					break;
					
					
					case SELECT_TIME_MAX:																	//Seleccion de Tmax
					if((t_obj == 0 && temp == 1) || (t_obj > 0 && t_obj < MAX_TIME_OBJ)){
						t_obj += temp;
					} else {
						t_obj = 0;
					}
					printSubMenuIConst(11, 0);
					break;
					
					
					case INICIAR:																			//Seleccion de Iniciar?
					if(temp == 1){
						subMenu = CANCELAR;
						printSubMenuIConst(13, 1);
					}
					break;
					
					case CANCELAR:																			//Seleccion de Cancelar?
					if(temp == -1){
						subMenu = INICIAR;
						printSubMenuIConst(12, 1);
					}
					break;
					
					case RUN_I_CONST_A:																		//Pantalla de Running I Constante (Actualiza la Iobj en vivo)
					case RUN_I_CONST_B:
					if(I_obj == CURRENT_MIN_SALTOS_RUNNING && temp == 1){
						I_obj += temp * CURRENT_MIN_SALTOS_RUNNING;
					} else if(I_obj > CURRENT_MIN_SALTOS_RUNNING && I_obj < MAX_CURRENT){
						I_obj += temp * CURRENT_MIN_SALTOS_RUNNING;
					}
					if(!DAC_setVoltaje( (uint16_t) I_TO_DAC(I_obj))){
						printErrorDAC();
					}
					break;

					default: break;
				}
			}
			
			//Acciones al presionar OK
			if(readButton()){
				switch(subMenu){
					case SELECT_I:
					subMenu = SELECT_TIME_MAX;
					printSubMenuIConst(11, 1);
					break;
					
					case SELECT_TIME_MAX:
					subMenu = INICIAR;
					printSubMenuIConst(12, 1);
					break;
					
					case INICIAR:
					subMenu = RUN_I_CONST_A;
					/************************************************************************/
					/*					INSTANTE PREVIO A INICIALIZAR                       */
					/* Las siguientes instrucciones inicializan las variables para lograr	*/
					/* leer correctamente las magnitudes de corriente, capacidad y tiempo	*/
					/************************************************************************/
					mark_t_zero = millis();
					mark_dt_mah = mark_t_zero;
					I_mAh = 0;
					if(!DAC_setVoltaje( (uint16_t) I_TO_DAC(I_obj))  ){
						printErrorDAC();
					} else {
						printSubMenuIConst(14, 1);
					}
					break;
					
					case CANCELAR:
					menu = HOME;
					printMenu(1);
					DELAY_MENU_EXIT();
					break;
					
					case RUN_I_CONST_A:
					case RUN_I_CONST_B:
					if(!DAC_setVoltaje(0)){
						printErrorDAC();
					}
					printMsjeFin();
					printMAH();
					printMenu(1);
					DELAY_MENU_EXIT();
					break;
					
					
					default: break;
				}
			}
			
			/************************************************************************/
			/*			EJECUCION CONTINUA -> MODO CORRIENTE CONSTANTE				*/
			/* El microcontrolador siempre llegara hasta if, mientras no ocurran	*/
			/* fallas, o el tiempo se halla completado, o el usuario presiono el	*/
			/* boton.																*/
			/************************************************************************/
			if(subMenu == RUN_I_CONST_A || subMenu == RUN_I_CONST_B){
				static unsigned long now = 0;
				static unsigned long mark1 = 0;		//Cada 250ms -> Imprime en pantalla
				static unsigned long mark2 = 0;		//Cada 10ms  -> Control de estados
				static unsigned long mark3 = 0;		//			 -> Control de falla por no alcanzar la corriente
				static unsigned long mark4 = 0;		//Cada 5  s	 -> Para cambiar los datos en pantalla
				
				now = millis();
				t_acum = (now - mark_t_zero)/1000;				//Segundos
				I_read = CURRENT_ADC_TO_MA(I_read_raw);			//mA
				T_read = TEMP_CONVERT_TO_CELCIUS(TEMP_ADC_TO_RNTC(T_read_raw));
				
				if(now - mark1 > 250){			//Cada 250ms
					mark1 = now;	
					printSubMenuIConst(subMenu == RUN_I_CONST_A ? 14 : 15, 0);
				}
				

				if(now - mark2 > 10){			//Cada 10ms - Control de Estados
					mark2 = now;
					
					//Detengo si sobrepasa la temperatura maxima				
					if(T_read > MAX_TEMP){
						if(!DAC_setVoltaje(0)){
							printErrorDAC();
						}
						printAlertMaxTemp();
						DELAY_MENU_EXIT();
					}
					
					//Detengo si sobrepasa la corriente maxima
					if(I_read > MAX_HARDWARE_CURRENT){
						if(!DAC_setVoltaje(0)){
							printErrorDAC();
						}
						printAlertMaxCurrent();
						DELAY_MENU_EXIT();
					}
					
					//Detengo si llego al tiempo especificado
					if(t_obj > 0){
						if(t_acum > t_obj*60){
							if(!DAC_setVoltaje(0)){
								printErrorDAC();
							}
							printMsjeTiempo();
							printMAH();
							DELAY_MENU_EXIT();
						}
					}
					
					//Cambia lo que se muestra en pantalla
					if(now - mark4 >= 5000){
						mark4 = now;
						if(t_acum > 5) subMenu = subMenu == RUN_I_CONST_A ? RUN_I_CONST_B : RUN_I_CONST_A;
					}
					
					
					//Se excede X cant, durante Z tiempo, encontes corta
					if(abs(I_read - I_obj) > MAX_ERROR_CURRENT && t_acum > 5){	
						if(now - mark3 > MAX_TIME_ERROR_CURRENT){
							if(!DAC_setVoltaje(0)){
								printErrorDAC();
							}
							printMsjeCurrentFail();
							while(!readButton());
							menu = HOME;
							printMenu(1);
							DELAY_MENU_EXIT();
						}
					} else {
						mark3 = mark2;
					}
				}
				
				
				
				
			}
			
			
			break;
			
			
			/************************************************************************/
			/*					SUBMENU -> CORRIENTE POR RAMPA                      */
			/* Todavia no se implemento, pero seria algo parecido al submenu		*/
			/* anterior ya que se deberia seleccionar los saltos de tiempo y la		*/
			/* corriente maxima, o el tiempo maximo y la corriente maxima.			*/
			/************************************************************************/
			case SUB_RAMP_CURRENT:
			
			break;
			default: break;
		}
    }
}




/************************************************************************/
/*						FUNCION MILLIS()                                */
/* Simplemente utiliza una interrupcion para incrementar en 1 una		*/
/* variable llamada 'millis_counter'. Luego la funcion devuelve este	*/
/* valor con el cuidado de desactivar las interrupciones cuando accede	*/
/* a su valor. Luego las devuelve al estado que tenia.					*/
/************************************************************************/
unsigned long millis() {
	unsigned long millis_value;

	//CLI() -> Ejecuto -> SEI()
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_value = millis_counter;
	}

	return millis_value;
}
ISR(TIMER1_COMPA_vect) {
	millis_counter++;
}




/************************************************************************/
/*		  			LECTURAS DEL ENCODER ROTATIVO                       */
/* Simplemente detecta el flanco de bajada de uno de los canales (si	*/
/* existe, es porque se giro), y luego detecta el estado logico del		*/
/* otro canal (si es 1 o 0, significa un sentido o el otro).			*/
/* La funcion del pulsador es simplemente detectar el cambio de 0 a 1.	*/
/************************************************************************/
int8_t readEncoder(){
	int8_t result = 0;
	static uint8_t prev_CLK = 0;
	
	uint8_t now_CLK = (PIND & (1 << PORTD7));
	uint8_t now_DT = (PINB & (1 << PORTB0));
	
	if(prev_CLK != now_CLK){
		_delay_ms(5);
		prev_CLK = now_CLK;
		
		if(now_CLK == 0){
			if(now_DT == 0){
				result = INV_LOGICA_ENCODER ? 1 : -1;
			} else if(now_DT == 1){
				result = INV_LOGICA_ENCODER ? -1 : 1;
			}
		}
	}
	
	return(result);
}
uint8_t readButton(){
	static uint8_t bt_prev = 0;
	uint8_t bt_now = BUTTON();
	uint8_t result = 0;
	
	if(bt_now != bt_prev){
		_delay_ms(3);
		if(bt_now == 1 && bt_prev == 0){
			result = 1;
		}
		
		bt_prev = bt_now;
	}
	
	return(result);
}





/************************************************************************/
/*			LECTURAS DEL ADC -> CORRIENTE Y TEMPERATURA                 */
/* En esta funcion, la principal es el ISR que es en esta donde se		*/
/* detecta el canal que lanzo la interrupcion y segun este, opero sobre	*/
/* el valor leido. Y ademas, reconfiguro el disparo para el otro canal.	*/
/* Por motivos de mejorar la lectura, se realiza un promedio de 15		*/
/* lecturas para poder recien ahi, almacenarlas en la variable global	*/
/* que utiliza el programa principal.									*/
/* Ademas, si estoy en modo "Running", entonces voy acumulando los		*/
/* mA*dt para luego obtener los mAh.									*/
/************************************************************************/
void ADC_readInit(){
	ADC_readCurrent();
}
void ADC_readCurrent(){
	ADMUX &= ~((1 << MUX3) | (1 << MUX2));	//PC3 [A3]
	ADMUX |=  (1 << MUX1) | (1 << MUX0);
	ADCSRA |= (1 << ADSC);
}
void ADC_readTemperature(){
	ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));  //PC0 [A0]
	ADCSRA |= (1 << ADSC);
}
ISR(ADC_vect){
	uint16_t lectura = ADC;
	
	if((ADMUX & 0x0F) == 0x03){ //A3 -> Leo Corriente
		if(I_read_prom_count >= CURRENT_PROM_CANT){
			I_read_raw = I_read_raw_acum/CURRENT_PROM_CANT;
			I_read_prom_count = 0;
			I_read_raw_acum = 0;
			
			//Acumula los mA*dt para luego obtener los mAh al dividir por 3600*1000
			if(subMenu == RUN_I_CONST_A || subMenu == RUN_I_CONST_B){
				I_mAh += I_read * (millis_counter - mark_dt_mah) / 3600000.0;
				mark_dt_mah = millis_counter;
			}
			
		} else {
			I_read_raw_acum += lectura;
			I_read_prom_count++;
		}
		ADC_readTemperature();
	} else if((ADMUX & 0x0F) == 0x00){ //A0 -> Leo Temperatura
		if(T_read_prom_count >= TEMP_PROM_CANT){
			T_read_raw = T_read_raw_acum/TEMP_PROM_CANT;
			T_read_prom_count = 0;
			T_read_raw_acum = 0;
		} else {
			T_read_raw_acum += lectura;
			T_read_prom_count++;
		}
		ADC_readCurrent();
	}
}





/************************************************************************/
/*				IMPRESIONES EN LCD -> MENU PRINCIPAL                    */
/* Son las funciones que simplifican a la vista el programa principal.	*/
/* Cada una muestra un mensaje especifico, y depende de cual sea, tiene	*/
/* algun delay o espera el pulsador.									*/
/************************************************************************/
void printMenu(uint8_t clean){
	if(clean){
		LCD_Clear();
		LCD_printString("MENU PRINCIPAL");
	} else {
		LCD_setCursor(0, 1);
		LCD_printString("                ");
	}
	
	
	LCD_setCursor(0, 1);
	if(subMenuIndex > 0){
		LCD_printString("< ");
	}
	LCD_printString(submenu_title[subMenuIndex]);
	
	if(subMenuIndex < SUB_MAX){
		LCD_setCursor(15, 1);
		LCD_printString(">");
	}
}

void printMsjeFactoryReset(){
	LCD_Clear();
	LCD_printString("  Espere");
	LCD_setCursor(5, 1);
	LCD_printString("Por Favor!");
	_delay_ms(3000);
	LCD_Clear();
	LCD_printString(" Reiniciar");
	LCD_setCursor(3, 1);
	LCD_printString("Dispositivo!");
	while(1);
}

/************************************************************************/
/*			IMPRESIONES EN LCD -> SUBMENU, CORRIENTE CONSTANTE          */
/* Son todas las pantallas que aparecen en el submenu de Corriente Cte.	*/
/************************************************************************/
void printSubMenuIConst(uint8_t index, uint8_t clean){
	if(clean && index != 14){
		LCD_Clear();
		LCD_printString("Corr. Constante");
	}
	
	LCD_setCursor(0, 1);
	
	if(index ==  10){												//Configura los mA
		if(clean) LCD_printString("I:     0000  mA");
		LCD_setCursor(7, 1);
		if(I_obj < 10					) LCD_printString("000");
		if(I_obj < 100	&& I_obj >= 10	) LCD_printString("00");
		if(I_obj < 1000 && I_obj >= 100	) LCD_printString("0");
		LCD_printInt(I_obj);	
	} else if(index == 11){											//Configura el Tiempo maximo
		if(clean) LCD_printString("Tmax:   000  m");
		LCD_setCursor(8, 1);
		if(t_obj == 0){
			LCD_printString("---");
		} else {
			if(t_obj < 10					) LCD_printString("00");
			if(t_obj < 100	&& t_obj >= 10	) LCD_printString("0");
			LCD_printInt(t_obj);	
		}
	} else if(index == 12){											//Pregunta: iniciar?
		if(clean) LCD_printString("Iniciar?");
	} else if(index == 13){											//Pregunta: cancelar?
		if(clean) LCD_printString("Cancelar?");
	} else if(index == 14){											//Pantalla "Running" mostrando mAh
		if(clean) {
			LCD_Clear();
		}
		//Corriente Actual/Corriente Objetivo
		LCD_setCursor(0,0);
		LCD_printString("I:");
		LCD_printFloat(I_read/1000.0, 5, 3);
		LCD_printString("/");
		LCD_printFloat(I_obj/1000.0, 5, 3);
		LCD_printString(" A");
		
		//Tiempo y mAh
		LCD_setCursor(0, 1);
		
		static uint8_t  h = 0, m = 0;
		static uint16_t s = 0;
		
		h = t_acum / 3600;
		s = t_acum % 3600;
		m = s / 60;
		s = s % 60;
		
		//Completo con ceros si es necesario
		if(h < 10) LCD_printString("0");
		LCD_printInt(h);
		LCD_printString(":");
		if(m < 10) LCD_printString("0");
		LCD_printInt(m);
		LCD_printString(":");
		if(s < 10) LCD_printString("0");
		LCD_printInt(s);
		LCD_printString(" ");
		
		//Si es menor a 9999mAh, entonces lo muestro asi tal cual
		//sino, entonces lo convierto a Ah
		if(I_mAh < 9999.0){
			if(I_mAh < 1000) LCD_printString(" ");
			if(I_mAh < 100 ) LCD_printString(" ");
			if(I_mAh < 10  ) LCD_printString(" ");
			LCD_printInt((uint16_t)I_mAh);
			LCD_printString("mAh");
		} else {
			if(I_mAh/1000.0 < 1000) LCD_printString(" ");
			if(I_mAh/1000.0 < 100 ) LCD_printString(" ");
			if(I_mAh/1000.0 < 10  ) LCD_printString(" ");
			LCD_printInt((uint16_t)(I_mAh/1000.0));
			LCD_printString(" Ah");
		}
		

		
		
	} else if(index == 15){											//Pantalla "Running" mostrando Temperatura
		if(clean) {
			LCD_Clear();
		}
		LCD_setCursor(0,0);
		LCD_printString("I:");
		LCD_printFloat(I_read/1000.0, 5, 3);
		LCD_printString("/");
		LCD_printFloat(I_obj/1000.0, 5, 3);
		LCD_printString(" A");
		
		//Tiempo y Temperatura
		LCD_setCursor(0, 1);
		
		static uint8_t  h = 0, m = 0;
		static uint16_t s = 0;
		
		h = t_acum / 3600;
		s = t_acum % 3600;
		m = s / 60;
		s = s % 60;
		
		//Completo con ceros
		if(h < 10) LCD_printString("0");
		LCD_printInt(h);
		LCD_printString(":");
		if(m < 10) LCD_printString("0");
		LCD_printInt(m);
		LCD_printString(":");
		if(s < 10) LCD_printString("0");
		LCD_printInt(s);
		LCD_printString("  ");
		LCD_printFloat(T_read, 4, 1);
		LCD_printChar(223);
		LCD_printChar('C');
	}
	
	if(clean) _delay_ms(500);
	
}

void printAlertMaxCurrent(){
	menu = HOME;
	subMenu = SELECT_I;
	LCD_Clear();
	LCD_setCursor(1, 0);
	LCD_printString("MAXIMA  CORR.");
	LCD_setCursor(5, 1);
	LCD_printString("ALCANZADA!");
	while(!readButton());
	printMenu(1);
}

void printAlertMaxTemp(){
	menu = HOME;
	subMenu = SELECT_I;
	LCD_Clear();
	LCD_setCursor(1, 0);
	LCD_printString("MAXIMA  TEMP.");
	LCD_setCursor(5, 1);
	LCD_printString("ALCANZADA!");
	while(!readButton());
	printMenu(1);
}

void printMsjeFin(){
	menu = HOME;
	subMenu = SELECT_I;
	LCD_Clear();
	LCD_setCursor(1, 0);
	LCD_printString("CONSUMO");
	LCD_setCursor(4, 1);
	LCD_printString("DETENIDO!");
	_delay_ms(2000);
}

void printMsjeTiempo(){
	menu = HOME;
	subMenu = SELECT_I;
	LCD_Clear();
	LCD_setCursor(5, 0);
	LCD_printString("TIEMPO");
	LCD_setCursor(3, 1);
	LCD_printString("ALCANZADO!");
	while(!readButton());
}

void printMAH(){
	menu = HOME;
	subMenu = SELECT_I;
	
	static uint8_t  h = 0, m = 0;
	static uint16_t s = 0;
	
	h = t_acum / 3600;
	s = t_acum % 3600;
	m = s / 60;
	s = s % 60;
	
	LCD_Clear();
	LCD_printString("    ");
	if(h < 10) LCD_printString("0");
	LCD_printInt(h);
	LCD_printString(":");
	if(m < 10) LCD_printString("0");
	LCD_printInt(m);
	LCD_printString(":");
	if(s < 10) LCD_printString("0");
	LCD_printInt(s);
	
	LCD_setCursor(0, 1);
	LCD_printString("A:");
	LCD_printFloat(I_obj/1000.0, 5, 3);
	
	//Si es menor a 9999mAh, entonces lo muestro asi tal cual
	//sino, entonces lo convierto a Ah
	if(I_mAh < 9999.0){
		LCD_printString(" mAh:");
		if(I_mAh < 1000) LCD_printString(" ");
		if(I_mAh < 100 ) LCD_printString(" ");
		if(I_mAh < 10  ) LCD_printString(" ");
		LCD_printInt((uint16_t)I_mAh);
	} else {
		LCD_printString("  Ah:");
		if(I_mAh/1000.0 < 1000) LCD_printString(" ");
		if(I_mAh/1000.0 < 100 ) LCD_printString(" ");
		if(I_mAh/1000.0 < 10  ) LCD_printString(" ");
		LCD_printInt((uint16_t)(I_mAh/1000.0));
	}
	
	while(!readButton());
	printMenu(1);
}

void printMsjeCurrentFail(){
	menu = HOME;
	subMenu = SELECT_I;
	LCD_Clear();
	LCD_setCursor(1, 0);
	LCD_printString("ERROR  CORR.");
	LCD_setCursor(1, 1);
	LCD_printString("NO  ALCANZADA!");
	while(!readButton());
}


void printErrorDAC(){
	LCD_Clear();
	LCD_printString(" ERROR DE COMUN.");
	LCD_setCursor(3, 1);
	LCD_printString("CON EL DAC");
	while(1);
}