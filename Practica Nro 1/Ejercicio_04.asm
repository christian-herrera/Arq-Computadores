ORG 000

Start, 	LoadI pDIRb			/Inicializo el primer y segundo elemento en 0 y 1
        add One
        StoreI pDIRb
        
loop, 	loadI pDIRa			/AC <- Val(pDIRa)
		addI pDIRb			/AC <- AC + Val( Val(pDIRb) )
        Store Temp			/Temp <- AC
        
        JnS incrementDIR	/Incremento la posicion de los punteros
        load Temp			/Guardo el nuevo valor (la suma de los dos anteriores) en la
        StoreI pDIRb		/posicion de DIRb, que como se habia incrementado, seria el casillero siguiente
        
        load pDIRb			/Si la posicion donde guarde el ultimo elemento cae en 0x53 (Elemento 20)
        Subt DIRfin			/Resto 0x53, si da cero es porque estoy en esa posicion
        Skipcond 400		/AC == 0, Salto, es decir, si llege a 20 elementos, entonces finalizo
        Jump loop
        
        Halt
        
        
        
incrementDIR, 	HEX 000
				load pDIRb
                Store pDIRa
                Add One
                Store pDIRb
               	JumpI incrementDIR

                

pDIRa, HEX 40
pDIRb, HEX 41
DIRfin, HEX 53
Temp, DEC 0

One, DEC 1
Zero, DEC 0