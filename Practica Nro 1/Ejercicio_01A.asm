loop, 	Load NumX	/Obtengo el numero
		Add Num7
        Store NumX
        
        Output		/Lo imprimo
        
        StoreI DIR	/Guardo el valor y luego incremento la posicion
        Load DIR	
        Add One
        Store DIR	
        
        Load Cant   /Al contador, le voy restando 1
        Subt One
        Store Cant
        
        Skipcond 400    /Si conte 10 iteraciones, omito el bucle
		Jump loop
		
        
		Halt

NumX, DEC 0
Num7, DEC 7
Cant, DEC 10
One, DEC 1
DIR, HEX 50