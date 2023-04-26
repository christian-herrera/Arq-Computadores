ORG 00F

Jump Start

/Vector de Datos
CANT, DEC 11
X1, DEC -9
X2, DEC 3
X3, DEC -1
X4, DEC -5
X5, DEC 5
X6, DEC 15
X7, DEC 19
X8, DEC -5
X9, DEC 0
X10, DEC 45
X11, DEC -45

/Variables Internas
One, HEX 1
Count, DEC 0			/Contador de numeros negativos
pDIR, HEX 011			/Apunta a la direccion de memoria que se va a leer

Start,	LoadI pDIR		/Leo del puntero
		        
		Skipcond 000	/AC < 0 , Cuenta, Porque omite el salto a [Next]
        Jump Next
        
Cont, 	Load Count
		Add One
        Store Count

Next,   Load pDIR		/Incremento el apuntador
        Add One
        Store pDIR		
        Load CANT		/Decremento el contador de numeros a leer
		Subt One
        Store CANT
        Skipcond 400	/AC == 0, Finaliza, Porque omite el salto a [Start]
        Jump Start


Load Count
Output
Halt
