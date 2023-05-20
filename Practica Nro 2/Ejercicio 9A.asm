;
; Ejercicio9A.asm
;
; Created: 14/5/2023 12:17:56
; Author : christian
;


.INCLUDE "m328pdef.inc"

.DSEG					; Segmento de Datos
.ORG 0X0100				; Origen

.CSEG					; Segmento de Codigo
RJMP MAIN
MAIN:

.DEF COUNT1=R17
.DEF COUNT2=R18
.DEF COUNT3=R19


; Stack pointer
LDI R16, HIGH(RAMEND)
OUT SPL, R16
LDI R16, LOW(RAMEND)
OUT SPH, R16


LDI R16,(1 << PB5)	; Configuro el Pin 13 como OUTPUT
OUT DDRB, R16

Blink:
SBI PORTB,5			; ON
RCALL Delay			; Delay
CBI PORTB, 5		; OFF
RCALL Delay			; Delay
JMP Blink


Delay:
	LDI COUNT1, 26  ; 1 Clk
Loop3:
	LDI COUNT2, 30	; 1 Clk
Loop2:
	LDI COUNT3, 255	; 1 Clk
Loop:
	NOP				; 1 Clk
	DEC COUNT3		; 1 Clk
	BRNE Loop		; 2 Clk (Si vuelve a Loop), 1 Clk (Si Continua)

	DEC COUNT2		; 1 Clk
	BRNE Loop2		; 2 Clk (Si vuelve a Loop2), 1 Clk (Si Continua)

	DEC COUNT1		; 1 Clk
	BRNE Loop3		; 2 Clk (Si vuelve a Loop3), 1 Clk (Si Continua)
	
ret					; 4 Clk
