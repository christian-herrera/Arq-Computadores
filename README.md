# Assembler
Ejercicios hechos para la asignatura Arquitectura de Computadores (UNLP)
  
  

# Practica Nro. 1
## Ejercicio 1
Proponga un programa para MARIE que calcule, utilizando una iteración, los primeros 10 múltiplos de 7 y los almacene ordenados en forma creciente en la memoria, comenzando en la dirección 0x0050. Compare la iteración resultante con las estructuras de alto nivel conocidas (for, if, while, etc.). Compare con el código que resultaría si en lugar de utilizar una iteración, se utilizara sólo una secuencia de instrucciones.

  
## Ejercicio 2
Escriba un programa que calcule el largo de una cadena de caracteres que comienza en la dirección 0x0100 y termina con el carácter CR= 0x0D (Carriage Return, retorno de carro).

  

## Ejercicio 3
Escriba un programa para MARIE que determine la cantidad de números negativos en un bloque de datos. La longitud del bloque se encuentra almacenada en la dirección de memoria 0x0010 y el bloque comienza en la dirección 0x0011. Envíe el resultado a la salida.

  

## Ejercicio 4
Escriba un programa que calcule los primeros 20 elementos de la sucesión de Fibonacci y los almacene en orden creciente en posiciones consecutivas de memoria empezando en 0x0040. Verificación: El vigésimo término debe valer 0x1055 y debe quedar almacenado en la posición de memoria 0x53.

  

## Ejercicio 5
Implemente como subrutina el programa de multiplicación iterativa sin signo que se ofrece como ejemplo en el simulador y se discute en la guía de ejercicios resueltos. Utilice dos posiciones de memoria fijas para pasar los argumentos y una más para el resultado.

  

## Ejercicio 6
Escriba una subrutina que calcule el factorial de un número entero utilizando la subrutina de multiplicación del problema anterior. ¿Puede explotarse la recursividad de la operación factorial? Recuerde que: n! = n*(n-1)!

  

## Ejercicio Integrador
Proponga un programa que ordene en forma creciente un vector de números enteros de 16 bits en Ca2. La longitud del vector se encuentra almacenada en la posición de memoria 0x001 y el vector comienza en la dirección 0x002. El resultado del programa debe ser almacenado en la misma ubicación del vector original.

  
  

# Practica Nro. 2
## Ejercicio 9
Utilizando el LED disponible en el kit escriba un programa que lo prenda y apague 10 veces por segundo utilizando un retardo de 0.05 segundos con una rutina bloqueante (wasted cycle). ¿Qué desventaja presenta este método de temporización? ¿Cómo debería implementarse una rutina no-bloqueante? Implemente el inciso anterior de forma no bloqueante utilizando interrupciones y el módulo temporizador. ¿Cómo organizaría el programa para incorporar una tarea de background?

  

## Ejercicio 10
Un sistema tiene como objetivo detectar presencia mediante un sensor de barrera infrarroja (emular con un pulsador). Cuando el sistema detecta presencia debe emitir una señal intermitente de periodo 500ms por medio de una lámpara roja (emular con un led) durante 5 segundos. Implemente la detección del pulsador y la temporización mediante interrupciones. Teniendo en cuenta que el sistema funciona a batería, luego de realizar la secuencia de detección, el MCU debe entrar en modo bajo consumo hasta la próxima detección.

  

## Ejercicio Integrador
Implemente en AVR la solución propuesta en la Práctica 1 para la subrutina de ordenamiento de un vector. Se propone en este caso trabajar con un vector de largo máximo de 256 elementos de 8 bits. Se recomienda almacenar el largo del vector como una constante utilizando la directiva .SET del ensamblador, para poder modificarlo luego. El vector deberá ubicarse en la memoria RAM, comenzando en la dirección 0x0100.

Para que no sea exactamente el mismo problema agreguemos un desafío adicional:
 - Implementar una subrutina que complete el vector con números aleatorios. Esto permitirá, con múltiples corridas, calcular el tiempo medio, máximo y mínimo que demora el algoritmo en ordenarlo.

Verifique el correcto funcionamiento en el simulador. Mida su performance y la utilización de memoria. Compare con la solución obtenida para MARIE.
La performance de su solución quedará caracterizada por los siguientes parámetros, los cuales deberán ser indicados claramente en el informe:

 - Tiempo medio de la iteración generación-ordenamiento del vector de 256 elementos (en ms)
 - Ocupación de la memoria de programa (en palabras de 16 bits)
 - Ocupación de la memoria de datos (en bytes). No incluir el vector.