HOME, 	clear
        load Zero
        StoreI pRes			/Limpio el resultado del multiplicador
		INPUT
        Store N				/Obtengo N
		
		subt Tree			/Si N=2 o N=1, devuelvo el mismo N, caso contrario hago Jump a [Init]
        Skipcond 000
        Jump Init
        Add Tree
        output
        halt
        
        
        
Init,   Add Tree			
		StoreI pNumA		/ *pNumA <- N
        subt One			
        StoreI pNumB		/ *pNumB <- N-1
        JnS MultiplicarAB	/ N * (N - 1)
        Load One			
        Store Count			/ Count <- 1

        
/La idea es ir haciendo:  	*pRes <- N * (N - 1)					Luego Count <- 1   	(Esto lo hace [Init])
/							*pRes <- (*pRes)(N - Count{1} - 1)		Luego Count <- 2	(Esto lo hace el bucle [loop])
/							*pRes <- (*pRes)(N - Count{2} - 1)		Luego Count <- 3	
loop, 	loadI pRes			
		StoreI pNumA	/ *pNumA <- *pRes
		load N
        subt Count
        subt One
        StoreI pNumB	/ *pNumB <- N - Count - 1
        load Zero
        StoreI pRes		/ Limpio *pRes
             
        JnS MultiplicarAB
        
        /Incremento Count
        load Count
        add One
        Store Count
        
        /Compruebo que llego a (N - Count - 1) = (0)
        /Como esta valor provoca que la multiplicacion de cero, entonces finalizo antes
        /Si es asi, entonces no vuelvo al [loop]
        load N
        subt Count
        subt One
		Skipcond 400
        Jump loop
        
        /Imprimo la ultima multiplicacion, esta seria *pRes
        loadI pRes
        output
        halt
        




MultiplicarAB, 	HEX FFFF
Loop,			LoadI pRes		/AC <- *pRes
                AddI pNumA		/AC <- AC + *pNumA
 				StoreI pRes		/*pRes <- AC    ------->   *pRes = *pRes + *pNum
                
                loadI pNumB		/Por cada vez que realizo una suma, decremento en uno el operando B
                subt One
                StoreI pNumB
                Skipcond 400	/*pNum == 0, Fin, Omito el salto a [Loop]
                Jump Loop
                
                JumpI MultiplicarAB
               

pNumA, HEX 50
pNumB, HEX 51
pRes, HEX 52

N, DEC 0
Count, DEC 0

Tree, DEC 3
Two, DEC 2
One, DEC 1
Zero, DEC 0