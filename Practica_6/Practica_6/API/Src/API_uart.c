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


extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_tx;

void uartInit(void)
{

}
