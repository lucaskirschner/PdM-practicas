/*
 * API_huart.c
 *
 *  Created on: Apr 3, 2025
 *      Author: Lucas Kirschner
 */

#include <API_uart.h>
#include "stm32f4xx_hal.h"
#include "string.h"

#define UART_MAX_SIZE 256

extern UART_HandleTypeDef huart2;

static void _uart_Error_Handler(void);

bool_t uartInit(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		return false;
	}

	const char *initMsg = "UART2 initialized @115200 8N1\r\n";
	uartSendString((uint8_t *)initMsg);

	return true;
}

void uartSendString(uint8_t *pString)
{
	if (pString == NULL) return;

	if(HAL_UART_Transmit(&huart2, pString, (uint16_t) strlen((char *)pString), HAL_MAX_DELAY) != HAL_OK)
	{
		_uart_Error_Handler();
	}
}

void uartSendStringSize(uint8_t *pString, uint16_t size)
{
	if (pString == NULL || size == 0 || size > UART_MAX_SIZE) return;

	if(HAL_UART_Transmit(&huart2, pString, size, HAL_MAX_DELAY) != HAL_OK)
	{
		_uart_Error_Handler();
	}
}

void uartReceiveStringSize(uint8_t *pString, uint16_t size)
{
	if (size == 0 || size > UART_MAX_SIZE) return;

	if(HAL_UART_Receive(&huart2, pString, size, HAL_MAX_DELAY) != HAL_OK)
	{
		_uart_Error_Handler();
	}
}

static void _uart_Error_Handler(void)
{
	  __disable_irq();
	  while (1){}
}

