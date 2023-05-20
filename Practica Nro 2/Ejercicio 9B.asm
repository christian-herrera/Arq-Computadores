;
; AssemblerApplication1.asm
;
; Created: 14/5/2023 12:17:56
; Author : christian
;


;Led que enciende y apaga 10 veces por segundo, sin bloqueo
;######################################################################################################################
;GLOBALES

.DSEG							;Datos
.ORG 0X0100						;Origen

.DEF CONF = R16					;R16 Configuraciones
.DEF bkSREG = R17				;R17 Para la pila

.SET DEC_OCR = 12500			;Es el valor en el cual dispara la interrupcion

;######################################################################################################################
;GENERAL

.CSEG							;Codigo
.ORG 0x0000						;Origen
RJMP MAIN

.ORG	0x16					;0x16 Corresponde al ISR del Comparador A del Timer 1
RJMP	ISR_COMPA				;Llamo a la rutina
MAIN:

;######################################################################################################################
;STACK POINTER, Pila en el final de la memoria
LDI CONF,HIGH(RAMEND)
OUT	SPH,CONF
LDI CONF,LOW(RAMEND)
OUT	SPL,CONF

;######################################################################################################################
;INPUT/OUTPUT
	LDI CONF,(1<<DDB5)
	OUT	DDRB,CONF
	
;TIMER1
	LDI	CONF,0
	STS	TCCR1A,CONF					; Modo OCN y OCnX desconectado

	LDI	CONF,(1<<CS11)|(1<<CS10)|(1<<WGM12)	
	STS TCCR1B,CONF					; Prescaler N=64 y Modo OCN		

	LDI	CONF,(1<<OCIE1A)
	STS	TIMSK1,CONF					; Interrupcion por comparacion con OCR1A

	LDI CONF, HIGH(DEC_OCR)
	STS OCR1AH,CONF
	LDI CONF,LOW(DEC_OCR)
	STS OCR1AL,CONF					; Valor con el que compara el contador

;/TIMER1
	
	SEI								; SREG[7] = 1 -> Habilito las interrupciones globales

;######################################################################################################################
;PROGRAMA PRINCIPAL

LOOP:
	RJMP LOOP


;######################################################################################################################
;INTERRUPCION
.SET MASK = (1 << PORTB5)
ISR_COMPA:		
	IN R19, PORTB	; Simplemente, invierte el bit del Led
	LDI r20, MASK
	EOR R19, R20
	OUT PORTB, R19
	RETI

;######################################################################################################################

