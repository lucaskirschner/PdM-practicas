/*
 * API_huart.h
 *
 *  Created on: Apr 3, 2025
 *      Author: Noxie
 */

#ifndef INC_API_UART_H_
#define INC_API_UART_H_

#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;

bool_t uartInit(void);
void uartSendString(uint8_t * pString);
void uartSendStringSize(uint8_t * pString, uint16_t size);
void uartReceiveStringSize(uint8_t * pString, uint16_t size);

#endif /* INC_API_UART_H_ */
