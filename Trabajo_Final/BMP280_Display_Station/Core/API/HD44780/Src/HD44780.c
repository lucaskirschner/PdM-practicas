/**
 * @file HD44780.c
 * @author Lucas Kirschner
 * @date 10 Apr 2025
 * @brief Implementación de funciones para controlar el LCD HD44780.
 *
 * Este archivo contiene las definiciones de las funciones necesarias para controlar
 * el display LCD basado en el controlador HD44780, incluyendo la inicialización,
 * escritura de caracteres y limpieza del display.
 */

#include "HD44780.h"
#include <stdio.h>


/* Retardos de ejecución de operaciones */
#define DELAY_TIME_1MS		1				/* Delay de  1ms */
#define DELAY_TIME_2MS		2				/* Delay de  2ms */
#define DELAY_TIME_10MS		10				/* Delay de 10ms */
#define DELAY_TIME_40MS		40				/* Delay de 40ms */



/**
 * @brief Inicializa el controlador LCD HD44780 en modo de 4 bits.
 *
 * Esta función realiza la secuencia de inicialización recomendada por la hoja de datos
 * del HD44780 para configurar la interfaz en modo de 4 bits. Aunque se usan sólo los pines
 * D4-D7, el controlador LCD arranca por defecto en modo de 8 bits. Por lo tanto, es necesario
 * enviar tres veces el comando Function Set (0x03) como si se tratara de una interfaz de 8 bits.
 *
 * Dado que sólo están conectados los pines D4-D7, se transmite únicamente el nibble alto 0x03,
 * el cual el LCD interpreta como 0x30 (modo 8 bits). El LCD asume que el nibble bajo es 0 porque
 * no hay señal conectada en D3-D0.
 *
 * Luego, se envía el nibble 0x02 para indicar que se usará modo 4 bits de forma definitiva.
 * A partir de ese punto, todos los comandos se envían en dos nibbles (alto y bajo).
 *
 * La secuencia incluye los siguientes pasos:
 * 0. Inicialización del periférico
 * 1. Espera inicial tras el encendido (más de 15 ms después de que VCC haya subido a 4.5V).
 * 2. Envío de tres comandos para configurar el controlador en modo de 8 bits.
 * 3. Cambio a modo de 4 bits.
 * 4. Configuración de parámetros de operación como número de líneas y tipo de fuente.
 * 5. Apagado del display.
 * 6. Limpieza del display.
 * 7. Encendido del display.
 * 8. Configuración del modo de entrada (incremento de la dirección y sin desplazamiento de la pantalla).
 *
 * @note Es importante respetar los tiempos mínimos entre comandos para asegurar
 * la correcta inicialización del LCD.
 *
 * @see HD44780 datasheet, sección "Initialization"
 */
hd44780_status_t HD44780_Init(void)
{
	/* Paso 0: Inicialización del periférico */
	if(HD44780_Port_Init() != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;

	/* Paso 1: Espera después de que VCC haya subido (más de 15 ms) */
	HD44780_Port_Delay(DELAY_TIME_40MS); 				/* Se suele usar 40 ms por seguridad */

	/* Paso 2: Envío de los primeros comandos (modo 8 bits por defecto) */
	if(HD44780_Port_Send_Nibble(0x03, false) != HD44780_PORT_OK)		/* LCD interpreta como 0x30 (modo 8 bits) */
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_10MS);								/* Espera >4.1 ms */

	if(HD44780_Port_Send_Nibble(0x03, false) != HD44780_PORT_OK)		/* Repetición para reforzar el modo 8 bits */
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_1MS);									/* Espera >100us */

	if(HD44780_Port_Send_Nibble(0x03, false) != HD44780_PORT_OK)		/* Repetición para reforzar el modo 8 bits */
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_1MS);									/* Espera >100us */

	/* Paso 3: Enviar comando 0x20 → Modo de 4 bits */
	if(HD44780_Port_Send_Nibble(0x02, false) != HD44780_PORT_OK)		/* LCD interpreta como 0x20 (modo 4 bits) */
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_1MS);

	/* Paso 4: Configurar el modo de operación (líneas, fuente, etc.) */
	if(HD44780_Port_Send_Byte(IR_FUNCTION_SET(LCD_INTERFACE_4BIT, LCD_DISPLAY_2LINE, LCD_FONT_5x8), false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;

	/* Paso 5: Apagar el display */
	if(HD44780_Port_Send_Byte(IR_DISPLAY_CONTROL(LCD_DISPLAY_OFF, LCD_CURSOR_OFF, LCD_BLINK_OFF), false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;

	/* Paso 6: Limpiar el display */
	if(HD44780_Port_Send_Byte(IR_CLEAR_DISPLAY, false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_2MS);					/* Esperar al menos 2 ms para que el display se limpie */

	/* Paso 7: Encender el display */
	if(HD44780_Port_Send_Byte(IR_DISPLAY_CONTROL(LCD_DISPLAY_ON, LCD_CURSOR_OFF, LCD_BLINK_OFF), false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;

	/* Paso 8: Configurar el modo de entrada (incremento y no desplazamiento) */
	if(HD44780_Port_Send_Byte(IR_ENTRY_MODE_SET(LCD_ENTRY_INCREMENT, LCD_ENTRY_SHIFT_OFF), false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;

	return HD44780_OK;
}

hd44780_status_t HD44780_Write(uint8_t *dataWrite)
{
	if(dataWrite == NULL)
		return HD44780_ERROR_PARAM;				/* Puntero nulo */

	while (*dataWrite != '\0')
	{
		if(HD44780_Port_Send_Byte(*dataWrite, true) != HD44780_PORT_OK)
			return HD44780_ERROR_COMM;
		dataWrite++;
	}

	return HD44780_OK;
}

hd44780_status_t HD44780_Clear(void)
{
	if(HD44780_Port_Send_Byte(IR_CLEAR_DISPLAY, false) != HD44780_PORT_OK)
		return HD44780_ERROR_COMM;
	HD44780_Port_Delay(DELAY_TIME_2MS);			/* Esperar al menos 2 ms para que el display se limpie */

	return HD44780_OK;
}

hd44780_status_t HD44780_Set_Cursor(uint8_t row, uint8_t column)
{
    if ((row < 1) || (row > 2) || (column < 1) || (column > 16))
        return HD44780_ERROR_PARAM; 			/* Parámetros fuera de rango */

    uint8_t address;

    if (row == 1)
    {
        address = 0x00 + (column - 1); 			/* Fila 1 empieza en 0x00 */
    }
    else
    {
        address = 0x40 + (column - 1); 			/* Fila 2 empieza en 0x40 */
    }

    if(HD44780_Port_Send_Byte(IR_SET_DDRAM_ADDR(address), false) != HD44780_PORT_OK)
    	return HD44780_ERROR_COMM;

    return HD44780_OK;
}

hd44780_status_t HD44780_Write_int(int16_t value)
{
	hd44780_status_t status;

	char buffer[7]; 							/* -32768\0 -> máximo 6 caracteres + nulo */

	sprintf(buffer, "%d", value);				/* Convierte el número a string decimal */
	status = HD44780_Write((uint8_t *)buffer);	/* Envía el string al display */

	return status;
}
