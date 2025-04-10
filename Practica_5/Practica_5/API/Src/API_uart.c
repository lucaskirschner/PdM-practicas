/**
 * @file    API_uart.c
 * @author  Ing. Lucas Kirschner
 * @date    April 3, 2025
 * @brief   Implementación de la API de manejo de UART para STM32.
 *
 * Este archivo contiene la implementación de funciones para inicializar,
 * transmitir y recibir datos por UART. También incluye funciones auxiliares
 * privadas para interpretar los parámetros de configuración de la UART.
 *
 * @note    Asegúrese de haber configurado correctamente el handler UART
 *          antes de usar esta API.
 */

#include "API_uart.h"
#include <string.h>
#include <stdio.h>

#define UART_MAX_SIZE 128  /**< Tamaño máximo para buffers de transmisión/recepción */
#define UART_DELAY_MS 10   /**< Delay en ms de recpeción uart						*/

/**
 * @brief Obtiene el largo de palabra configurado en la UART como carácter.
 * @param phuart Puntero al handler UART.
 * @return '8' para 8 bits, '9' para 9 bits, '?' si no se reconoce.
 */
static const char getWordLength(UART_HandleTypeDef *phuart)
{
    switch (phuart->Init.WordLength)
    {
        case UART_WORDLENGTH_8B: return '8';
        case UART_WORDLENGTH_9B: return '9';
        default: return '?';
    }
}

/**
 * @brief Obtiene la configuración de paridad como carácter.
 * @param phuart Puntero al handler UART.
 * @return 'N' (None), 'E' (Even), 'O' (Odd), '?' si no se reconoce.
 */
static const char getParity(UART_HandleTypeDef *phuart) {
    switch (phuart->Init.Parity)
    {
        case UART_PARITY_NONE: return 'N';
        case UART_PARITY_EVEN: return 'E';
        case UART_PARITY_ODD:  return 'O';
        default: return '?';
    }
}

/**
 * @brief Obtiene la configuración de bits de stop como carácter.
 * @param phuart Puntero al handler UART.
 * @return '1' o '2' según los bits de stop configurados, '?' si no se reconoce.
 */
static const char getStopBits(UART_HandleTypeDef *phuart)
{
    switch (phuart->Init.StopBits)
    {
        case UART_STOPBITS_1: return '1';
        case UART_STOPBITS_2: return '2';
        default: return '?';
    }
}

/**
 * @brief Imprime por UART la configuración del puerto UART.
 * @param port Puntero a la estructura UART_Port_t que representa el puerto UART.
 */
static void printUartConfig(UART_Port_t *port) {
    char msg[UART_MAX_SIZE];

    snprintf(msg, sizeof(msg), "UART Initialized: @%lu %c%c%c\r\n",
             port->huart->Init.BaudRate,
             getWordLength(port->huart),
             getParity(port->huart),
             getStopBits(port->huart));
    uartSendString(port, (uint8_t *)msg);
}

bool_t uartInit(UART_Port_t *port, UART_HandleTypeDef *huart)
{
    if (port == NULL || huart == NULL) return false;

    port->huart = huart;
    port->initialized = true;

    printUartConfig(port);

    return true;
}

bool_t  uartSendString(UART_Port_t *port, uint8_t *pstring)
{
    if (port == NULL || pstring == NULL) return false;

    if (HAL_UART_Transmit(port->huart, pstring, strlen((char *)pstring), HAL_MAX_DELAY) != HAL_OK)
        return false;

    return true;
}

bool_t  uartSendStringSize(UART_Port_t *port, uint8_t *pstring, uint16_t size)
{
    if (port == NULL || pstring == NULL || size == 0 || size > UART_MAX_SIZE) return false;

    if (HAL_UART_Transmit(port->huart, pstring, size, HAL_MAX_DELAY) != HAL_OK)
    	return false;

    return true;
}

bool_t  uartReceiveStringSize(UART_Port_t *port, uint8_t *pstring, uint16_t size) {
    if (port == NULL || pstring == NULL || size == 0 || size > UART_MAX_SIZE) return false;

    if (HAL_UART_Receive(port->huart, pstring, size, UART_DELAY_MS) != HAL_OK)
    	return false;

    return true;
}

void terminalClearScreen(UART_Port_t *port)
{
	const char *clear_cmd = "\x1b[2J\x1b[H";			/**< Borra pantalla y lleva cursor al inicio	*/
	uartSendString(port, (uint8_t *)clear_cmd);

	printUartConfig(port);
}
