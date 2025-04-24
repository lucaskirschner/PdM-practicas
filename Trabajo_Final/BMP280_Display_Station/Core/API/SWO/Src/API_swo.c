/**
 * @file API_swo.c
 * @author Ing. Lucas Kirschner
 * @date 29 Mar 2025
 * @brief Implementación de la función _write para redirigir la salida estándar (stdout) mediante SWO usando ITM_SendChar().
 *
 * Esta funcionalidad es útil para debug por consola sin necesidad de una UART física, solo con el depurador conectado al
 * microcontrolador STM32 compatible con ITM/SWO.
 */

#include "API_swo.h"
#include "stm32f4xx.h"

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
        ITM_SendChar(*ptr++);  		/**< Envía cada carácter a través del canal SWO */

    return len; 					/**< Devuelve la cantidad de caracteres enviados */
}
