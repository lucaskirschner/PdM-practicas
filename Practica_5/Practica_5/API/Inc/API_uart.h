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

typedef bool bool_t;

/**
 * @brief Estructura que representa un puerto UART.
 * Contiene el handler HAL y el estado de inicialización.
 */
typedef struct {
    UART_HandleTypeDef *huart;
    bool_t initialized;
} UART_Port_t;

/**
 * @brief Inicializa un puerto UART y guarda el handler en la estructura.
 * Además, transmite por UART una cadena con los parámetros actuales de configuración (baudrate, bits de datos, paridad, stop).
 * Ejemplo: "UART Initialized: @9600 8N1\r\n"
 *
 * @param port Puntero a una estructura UART_Port_t donde se almacenará el handler.
 * @param huart Puntero al handler de HAL UART a utilizar.
 * @return true si la inicialización fue exitosa, false si hubo punteros nulos.
 */
bool_t uartInit(UART_Port_t *port, UART_HandleTypeDef *huart);

/**
 * @brief Envía un string por UART hasta encontrar '\0'.
 * @param port Puntero al puerto UART.
 * @param pstring Puntero al string a enviar.
 * @return true si la transmisión fue exitosa, false si hubo error.
 */
bool_t uartSendString(UART_Port_t *port, uint8_t *pstring);

/**
 * @brief Envía un buffer por UART con una longitud dada.
 * @param port Puntero al puerto UART.
 * @param pstring Puntero al buffer a enviar.
 * @param size Número de bytes a enviar.
 * @return true si la transmisión fue exitosa, false si hubo error.
 */
bool_t uartSendStringSize(UART_Port_t *port, uint8_t *pstring, uint16_t size);

/**
 * @brief Recibe un string por UART de tamaño fijo.
 * @param port Puntero al puerto UART.
 * @param pstring Puntero al buffer donde almacenar la recepción.
 * @param size Número de bytes a recibir.
 * @return true si la recepción fue exitosa, false si hubo error.
 */
bool_t uartReceiveStringSize(UART_Port_t *port, uint8_t *pstring, uint16_t size);

#endif /* INC_API_UART_H_ */
