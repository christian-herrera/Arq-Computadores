# Ejercicio Integrador
En este ejercicio, implemente un dispositivo llamado "Carga Electronica". La misma es simplemente un circuito electrico que contiene un microcontrolador Atmega328, el cual permite extraer una corriente especifica de alguna fuente de tension que se desea probar.

## Esquematico
![Esquematico](https://github.com/christian-herrera/Arq-Computadores/tree/main/Practica%20Nro%203/Docs/Esquematico.svg)

El objetivo es lograr una tension fija en el terminal superior de la resistencia Shunt de 1 Ohm, para esto existe el MosFet como elemento de disipacion, liberando al Shunt de este trabajo. El amplificador operacional logra copiar la tension del DAC en el terminal superior del Shunt.

Ademas, se agregan perifericos como una pantalla LCD de 16x2, un Encoder Rotativo y Un Termistor NTC.

### Encoder Rotativo y LCD
Estos dos perifericos me permiten formar un menu basico donde puedo armar los diversos modos de funcionamiento, por ejemplo, Corriente Constante, Rampas de Corrientes, etc.

Ademas, puedo configurar las protecciones, tales como Limites de Tiempo, Corte por corriente maxima, Cortes por no alcanzar la corriente configurada, etc.

### Termistor NTC
El dispositivo que logra disipar la potencia sera el MosFet, pero este debera estar colocado sobre un disipador, por tal motivo, agrege una realimentacion que me devuelva la temperatura que esta teniendo el Mosfet en todo momento, la idea seria colocarlo lo mas cerca de este elemento.

### DAC MCP4725
Al momento de pensar en este periferico, las opciones eran, fabricar un DAC con un circuito RC que forme un pasa-bajos y obtenga una señal analogica con el menor ripple posible, o bien, usar este DAC de 12 bits. Dado que disponia del periferico, decidi incorporarlo al proyecto y simplemente para su funcionamiento, cree una libreria que se comunica por medio del protocolo I2C al periferico.


## Algoritmo

### Introduccion
El algoritmo me permite recorrer los diversos menus para la configuracion de los valores de corriente, tiempos. Una vez cargado estos valores, al presionar `Iniciar?`, entonces se inicializan las variables y se ejecuta en bucle el modo `Running`, el cual se describe en el siguiente item.

### Diagrama de Flujo (Modo Running)
![DiagFlujo](https://github.com/christian-herrera/Arq-Computadores/tree/main/Practica%20Nro%203/Docs/DiagFlujo.svg)
La forma de comprobar los milisegundos transcurridos es creando una interrupcion que logre incrementar en 1 una variable global cada 1 milisegundo, para esto utilice el timer de 16 bits y la interrupcion correspondiente al comparar el contador del timer con el valor en el registro OCR. Pasando por cada una de las preguntas del diagrama de flujo se logra llevar a cabo todas las tareas a realizar.