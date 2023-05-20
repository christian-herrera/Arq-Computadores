ORG 000

Start, 	clear
		INPUT					/Pido el numero A
		StoreI pNumA
        INPUT					/Pido el numero B
        StoreI pNumB
        Load Zero
        StoreI pRes
        
        JnS MultiplicarAB		/Llamo a la funcion Multiplicar 
        LoadI pRes				/Si finaliza, cargo el resultado y lo muestro
        Output
		Halt
        
        
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

Count, DEC 0
One, DEC 1
Zero, DEC 0