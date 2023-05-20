;
; AssemblerApplication1.asm
;
; Created: 18/5/2023 19:12:21
; Author : christian
;

.INCLUDE "m328pdef.inc"


;######################################################## Variables/Constantes
;Segmento de Datos
.DSEG
.ORG 0X100

.def CONF = R16             ;Se utiliza en configuraciones generales
.def pDIRa = R17            ;Para leer el elemento N
.def pDIRb = R18            ;Para leer el elemento N+1
.def COUNT_FIN = R19        ;Hasta donde lee en cada iteracion
.def COUNT = R20            ;Por cual elemento esta

.SET initPOS = 0x100        ;Posicion donde inicia el vector
.SET LONG_VECTOR = 0xFF		;Es la cantidad de valores
.SET valA = 0x03            ;Para el codigo pseudoaleatorio  new = a*old + b
.SET valB = 0x04






;######################################################## Generador de Numeros Pseudoaleatorios
;Segmento de Codigo
.CSEG
.ORG 0x00


;Pila
LDI CONF,HIGH(RAMEND)
OUT	SPH,CONF
LDI CONF,LOW(RAMEND)
OUT	SPL,CONF


;Generador de Numeros Pseudoaleatorios
LDI XL, LOW(initPOS)		
LDI XH, HIGH(initPOS)

LDI pDIRa,valA
LDI pDIRb,valB
LDI CONF,0x01		;Aleatorio
;LDI CONF,0xFF		;Desordenado
;LDI CONF,0x00		;Ordenado
ST X+,CONF
RJMP ALEATORIO


;Aleatorio
ALEATORIO:
	MUL CONF,pDIRa
	MOV CONF,R0
	ADD CONF,pDIRb
	ST X+,CONF
	INC COUNT
	CPI COUNT,(LONG_VECTOR  - 1)
	BRLO ALEATORIO
	RJMP START


;Desordenado
/*
ALEATORIO:
	DEC CONF
	ST X+,CONF
	INC COUNT
	CPI COUNT,LONG_VECTOR
	BRLO ALEATORIO
	RJMP START
*/

;Ordenado
/*
ALEATORIO:
	INC CONF
	ST X+,CONF
	INC COUNT
	CPI COUNT,LONG_VECTOR
	BRLO ALEATORIO
	RJMP START
*/



;######################################################## START
START:
	LDI XL,LOW(initPOS)			; X <- 0x100
	LDI XH,HIGH(initPOS)		
	LDI YL,LOW(initPOS + 1)		; Y <- 0x101
	LDI YH,HIGH(initPOS + 1)

	LDI COUNT, 0x00				; Primera pasada, COUNT contara la cantidad de pasadas completas
								; del vector


	LDI COUNT_FIN, (LONG_VECTOR)	; Indicara la cantidad de elementos que debo analizar del vector
									; a medida que completo el largo, le resto 1 COUNT_FIN
	
	RJMP Loop

Loop:
	LD pDIRa,X					; pDIRa	<- N	
	LD pDIRb,Y					; pDIRb <- N+1

	CP pDIRb,pDIRa		; Val(N+1) - Val(N)
	BRLO INVIERTO		; Val(N+1) < Val(N)  -> Invierto

Continuo:
	CPI COUNT_FIN,1
	BREQ Fin

	INC XL				; N++
	INC YL				; (N+1)++
	INC COUNT			; COUNT++

	CPSE COUNT,COUNT_FIN	;Si N+1 == N_fin
	RJMP Loop				;No reinicio el loop
	RJMP Reinicio			;sino que decremento el COUNT_FIN y luego vuelvo al loop



Reinicio:
	DEC COUNT_FIN
	CLR COUNT

	LDI XL,LOW(initPOS)			; X <- 0x100
	LDI XH,HIGH(initPOS)		
	LDI YL,LOW(initPOS + 1)		; Y <- 0x101
	LDI YH,HIGH(initPOS + 1)

	RJMP Loop	
	
	

INVIERTO:
	ST X, pDIRb         ; Val(N) <- Val(N+1)
	ST Y, pDIRa         ; Val(N+1) <- Val(N)
	LD pDIRa, X         ; Los vuelvo a leer
	LD pDIRb, Y
	RJMP Continuo
	

Fin:
	nop
	RJMP 0x00

