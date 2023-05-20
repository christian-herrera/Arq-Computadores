/ Algoritmo de ordenamiento en MARIE
/ Christian Yoel Herrera - 67095/8
/ Algoritmo utilizado: Metodo Burbuja
/ Ocupación de memoria de programa: 0x3D - 0x00 = 61 posiciones
/ Ocupación de memoria de datos (sin incluir el vector): 
/ Performance
/ Vector ordenado -3,-2,-1,0,1,2,3 : ... 422 instrucciones  (ciclos de reloj: 2954)
/ Vector invertido 3,2,1,0,-1,-2,-3 : ... 590 instrucciones (ciclos de reloj: 4130)
/ Vector desordenado 0,-2,1,3,-1,2,-3 : ... 510 instrucciones (ciclos de reloj: 3570)

ORG 000
Jump Start
VECTOR, DEC 7
DEC 0
DEC -2
DEC 1
DEC 3
DEC -1
DEC 2
DEC -3

/Para el algoritmo
pDIRa, HEX 2	/Indice el elemento N
pDIRb, HEX 3	/Indice del elemento N+1
posFIN, HEX 0	/Indica la posicion final hasta la que llega N+1

/Temporales y de Ayuda general
Temp, HEX 0

/Constantes
Zero, HEX 0
One, HEX 1
Two, HEX 2
Tree, HEX 3

Start, 	load pDIRa
        add VECTOR
        Store posFIN	/ posFIN <- pDIRa + Cant. Elementos (Posicion final = Ultimo elemento)
		Jump loop_burbujas

loop_principal, subt Tree
				Skipcond 400		/ posFIN - 0x03 = 0, omito el bucle de permutaciones
                Jump loop_burbujas
                halt
                

loop_burbujas, 	loadI pDIRa
                Store Temp
                loadI pDIRb
                subt Temp
                Skipcond 800	/ Val(N+1) > Val(N) => No permuto
                JnS Permutar

				/Incremento N, y por lo tanto N+1 (Son pDIRa y pDIRb)
                JnS IncrementDIRab	
				
                /Si N+1 == posFIN, entonces finalizo la pasada X
                load pDIRb
                subt posFIN				
                Skipcond 400			
                jump loop_burbujas
                
                /Reinicio N (son pDIRa=2 y pDIRb=3)
                Load Two			
                Store pDIRa
                load Tree
                Store pDIRb
				
                /Omito el ultimo elemento ya que este quedo ordenado
                /Esto lo logro haciendo que la pasada X+1 llege un elemento
                /menos que la pasada X, asi que reduzco en uno la cantidad
                /de elementos a analizar en la siguiente pasada.
                load posFIN
                subt One
                store posFIN
                
                /en el AC se mantiene posFIN, esto es para que cuando llego
                /a [loop_principal], simplemente uso este valor y no debo cargarlo
                
				Jump loop_principal

/ *pDIRa <= *pDIRa + 1
/ *pDIRb <= *pDIRb + 1
IncrementDIRab, HEX FFF
				load pDIRa
                Add One
                Store pDIRa
                load pDIRb
                Add One
                Store pDIRb
				JumpI IncrementDIRab

/Permuta los elementos a los que apunta pDIRa y pDIRb
Permutar, 	HEX FFF
			loadI pDIRa			
            Store Temp			/ Temp <- *pDIRa
           	loadI pDIRb
            StoreI pDIRa		/ *pDIRa <- *pDIRb
            load Temp
            StoreI pDIRb		/ *pDIRb <- Temp (es decir: *pDIRa)
            jumpI Permutar