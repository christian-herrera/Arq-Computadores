ORG 000

loop, 	JnS Increment
        
        loadI DIR
        subt CR
        Skipcond 400
        Jump loop
        Jump Fin

Increment, 	HEX FFFF
			load Cant
            Add One
            Store Cant

            load DIR
            Add One
            Store DIR
            JumpI Increment
           

Fin,	load Cant
		Output
		Halt


DIR, HEX 100
Cant, DEC 0
CR, HEX D
One, DEC 1