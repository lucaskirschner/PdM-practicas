/**
 * @file    API_uart.h
 * @author  Ing. Lucas Kirschner
 * @date    April 3, 2025
 * @brief   Interfaz de alto nivel para el manejo de la UART usando STM32 HAL.
 *
 * Esta API permite inicializar, enviar y recibir datos a través de un puerto UART.
 * Además, durante la inicialización se imprime automáticamente la configuración
 * actual (baudrate, bits de datos, paridad y bits de stop).
 *
 * @note    Compatible con STM32 y HAL drivers. Requiere que la UART sea inicializada
 *          previamente por HAL antes de llamar a uartInit().
 */

#ifndef INC_API_UART_H_
#define INC_API_UART_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

void uartInit(void);


#endif /* INC_API_UART_H_ */
