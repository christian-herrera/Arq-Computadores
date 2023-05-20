;
; AssemblerApplication1.asm
;
; Created: 18/5/2023 19:12:21
; Author : christian
;

.INCLUDE "m328pdef.inc"

;######################################################## Variables/Constantes
.def CONF = R16
.def READ = R17
.def COUNTER = R18


.SET DEC_OCR = 62500
.SET MAX_COUNT = 0x0A - 1		



;######################################################## Memorias
;Segmento de Datos
.DSEG
.ORG 0X100



;Segmento de Codigo
.CSEG
.ORG 0x0000
RJMP MAIN
.ORG 0x002
RJMP ISR_INT0
.ORG 0X001A
RJMP ISR_TOV1


;######################################################## MAIN
MAIN:

;Pila
LDI CONF,HIGH(RAMEND)
OUT	SPH,CONF
LDI CONF,LOW(RAMEND)
OUT	SPL,CONF


;Pines
SBI	DDRB,5					;PORTB[5] => OUTPUT
CBI PORTB,5					;PORTB[5] => LOW
	
CBI	DDRD,2					;PORTD[2] => INPUT
SBI PORTD,2					;PORTD[2] => PULL_UP


;Interrupciones
LDI CONF,(1 << ISC01)		;INT0 => Falling Edge
STS EICRA,CONF
LDI CONF,(1 << INT0)		;INT0 => Enable
OUT EIMSK,CONF


;Timer1A
LDI	CONF,0
STS	TCCR1A,CONF					;Modo OCN y OCnX desconectado

LDI	CONF,(1<<CS11)|(1<<CS10)|(1<<WGM12)	
STS TCCR1B,CONF					;Prescaler N=64 y Modo OCN		

LDI	CONF,(1<<OCIE1A)
STS	TIMSK1,CONF					;Interrupcion por comparacion con OCR1A

LDI CONF, HIGH(DEC_OCR)
STS OCR1AH,CONF
LDI CONF,LOW(DEC_OCR)
STS OCR1AL,CONF					;Valor con el que compara el contador


;Power-Down
LDI CONF,(1 << SM0)|(1 << SM1)|(1 << SM2)|(1 << SE)
OUT SMCR,CONF

;Habilito las interrupciones y voy al PreLoop
SEI
RJMP PreLoop

;######################################################## Loops
PreLoop:
	SLEEP
	RJMP Loop


Loop:
	CPI COUNTER,MAX_COUNT
	BREQ PreLoop
	RJMP Loop




;######################################################## Subrutinas
ISR_INT0:
	LDI COUNTER,0x0
	SBI PORTB,5
	RETI
	

ISR_TOV1:
	IN READ,PORTB
	LDI CONF, (1 << PB5)
    EOR CONF,READ
    OUT PORTB,CONF
	nop

	INC COUNTER

	RETI
	