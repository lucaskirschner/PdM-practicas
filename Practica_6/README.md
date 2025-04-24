# PdM-prácticas
Alumno: Lucas Kirschner
Práctica: 5

Implementación de una API para el manejo de UART utilizando HAL de STM32.

Se encapsulan las funciones necesarias en un archivo fuente API_uart.c con su correspondiente archivo de cabecera API_uart.h, ubicados dentro de las carpetas /API/src y /API/inc, respectivamente.

Se implementan las siguientes funciones públicas:

uartInit(): inicializa el puerto UART y transmite por terminal los parámetros de configuración.

uartSendString(): transmite una cadena de caracteres hasta \0.

uartSendStringSize(): transmite una cantidad fija de bytes desde un buffer.

uartReceiveStringSize(): recibe una cantidad fija de bytes desde UART.

Todas las funciones verifican punteros y parámetros, y utilizan las funciones de la HAL.

Se utiliza esta API en el programa principal para enviar mensajes por UART cuando se detectan flancos ascendentes o descendentes en un botón, utilizando la MEF anti-rebote implementada en la práctica 4.