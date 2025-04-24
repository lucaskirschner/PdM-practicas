Proyecto de Monitoreo de Temperatura y Presión con BMP280 y Display HD44780
Autor: Ing. Kirschner Lucas

Descripción
Este proyecto implementa un sistema de monitoreo de temperatura y presión utilizando el sensor BMP280. Los datos de temperatura y presión se muestran en un display LCD HD44780. El sistema está gestionado por una máquina de estados (FSM) que organiza los diferentes estados del proceso de medición, análisis y visualización. Además, se implementa un manejo de errores mediante la FSM, que incluye la reintentar la inicialización en caso de fallos.

Funcionalidades del Proyecto
Medición de temperatura y presión: Utiliza el sensor BMP280 para leer la temperatura (en grados Celsius) y la presión (en Pascales).
Visualización en Display LCD: Los valores de temperatura y presión se muestran en un display LCD HD44780.
Máquina de Estados: La máquina de estados organiza el flujo del programa, gestionando desde la inicialización de los periféricos hasta la visualización de los datos y el manejo de errores.
Manejo de Errores: En caso de un error en la lectura o en la inicialización de los periféricos, el sistema activa una alerta mediante el parpadeo de un LED y reinicia la inicialización tras un tiempo de espera.

Drivers Utilizados
1. Driver para BMP280 (BMP280.h)
El driver BMP280 se encarga de la configuración y la lectura de datos del sensor de temperatura y presión. Este driver facilita las siguientes funciones:
Inicialización del sensor: La función BMP280_Init verifica la conexión con el sensor y aplica la configuración estándar.
Disparo de mediciones: A través de la función BMP280_Trigger_Measurement, se inicia una nueva medición en el sensor.
Verificación del estado de medición: Con la función BMP280_Is_Measuring, se comprueba si los datos de medición están listos para ser leídos.
Lectura y compensación de datos: La función BMP280_Update_Parameters lee los registros crudos del sensor y aplica la compensación utilizando los coeficientes internos del BMP280, proporcionando los datos de temperatura y presión ya procesados.

2. Driver para Display LCD HD44780 (HD44780.h)
El driver HD44780 permite el manejo del controlador LCD, proporcionando funciones para controlar la pantalla y mostrar datos:
Inicialización del display: HD44780_Init configura la interfaz con el display.
Control de la posición del cursor: Las funciones HD44780_Set_Cursor permiten mover el cursor a la posición deseada en la pantalla.
Escritura de texto en el display: HD44780_Write y HD44780_Write_int permiten escribir cadenas de texto o valores enteros en el LCD.
Instrucciones de control: Implementa instrucciones IR del HD44780 como IR_CLEAR_DISPLAY para limpiar la pantalla o IR_RETURN_HOME para mover el cursor a la posición inicial.

Funcionamiento
El programa sigue los siguientes pasos:
Inicialización de componentes: Se inicializan el sensor BMP280 y el display HD44780. Si alguna de las inicializaciones falla, el sistema entra en un estado de error y parpadea un LED como indicación visual.
Medición de Datos: Una vez inicializados los componentes, el programa dispara una medición en el sensor BMP280.
Espera y Lectura de Datos: Se espera a que el sensor termine la medición. Al completarse, se leen los datos de temperatura y presión.
Análisis de Datos: Si los datos están fuera del rango de temperatura (25°C - 30°C), se indica una advertencia en el display.
Visualización de Datos: Los valores de temperatura y presión se muestran en el LCD. Si los valores de temperatura están fuera de rango, se muestra una advertencia en la pantalla.
Reintento en Caso de Error: Si ocurre un error en cualquier paso, el sistema entra en un estado de error, parpadeando un LED como señal, y después de un retraso, intenta reiniciar el proceso de inicialización.

Requisitos
Microcontrolador: STM32 (utiliza HAL para configuración de periféricos).
Sensor: BMP280.
Pantalla: LCD HD44780.
IDE: STM32CubeIDE o equivalente.
Herramientas de desarrollo: STM32CubeMX, HAL drivers.