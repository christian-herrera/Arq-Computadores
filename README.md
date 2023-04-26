# Assembler

Ejercicios hechos para la asignatura Arquitectura de Computadores (UNLP)



# Ejercicio 1
Proponga un programa para MARIE que calcule, utilizando una iteración, los primeros 10 múltiplos de 7 y los almacene ordenados en forma creciente en la memoria, comenzando en la dirección 0x0050. Compare la iteración resultante con
con las estructuras de alto nivel conocidas (for, if, while, etc.). Compare con el código que resultaría si en lugar de utilizar una iteración, se utilizara sólo una secuencia de instrucciones.

# Ejercicio 2
Escriba un programa que calcule el largo de una cadena de caracteres que comienza en la dirección 0x0100 y termina con el carácter CR= 0x0D (Carriage Return, retorno de carro).

# Ejercicio 3
Escriba un programa para MARIE que determine la cantidad de números negativos en un bloque de datos. La longitud del bloque se encuentra almacenada en la dirección de memoria 0x0010 y el bloque comienza en la dirección 0x0011. Envíe el resultado a la salida.

# Ejercicio 4
Escriba un programa que calcule los primeros 20 elementos de la sucesión de Fibonacci y los almacene en orden creciente en posiciones consecutivas de memoria empezando en 0x0040. Verificación: El vigésimo término debe valer 0x1055 y debe
quedar almacenado en la posición de memoria 0x53.

# Ejercicio 5
Implemente como subrutina el programa de multiplicación iterativa sin signo que se ofrece como ejemplo en el simulador y se discute en la guía de ejercicios resueltos. Utilice dos posiciones de memoria fijas para pasar los argumentos y una más para el resultado.

# Ejercicio 6
Escriba una subrutina que calcule el factorial de un número entero utilizando la subrutina de multiplicación del problema anterior. ¿Puede explotarse la recursividad de la operación factorial? Recuerde que: n! = n*(n-1)!

# Ejercicio Integrador
Proponga un programa que ordene en forma creciente un vector de números enteros de 16 bits en Ca2. La longitud del vector se encuentra almacenada en la posición de memoria 0x001 y el vector comienza en la dirección 0x002. El resultado del programa debe ser almacenado en la misma ubicación del vector original.