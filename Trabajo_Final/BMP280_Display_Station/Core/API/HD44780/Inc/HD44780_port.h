/**
 * @file HD44780_port.h
 * @author Ing. Lucas Kirschner
 * @date 10 Apr 2025
 * @brief Interfaz de bajo nivel para la comunicación con el controlador LCD HD44780 mediante I2C.
 *
 * Este archivo declara las funciones necesarias para inicializar y enviar datos al LCD HD44780,
 * encapsulando la lógica de transmisión en modo 4 bits sobre una interfaz I2C con expansor.
 */

#ifndef HD44780_INC_HD44780_PORT_H_
#define HD44780_INC_HD44780_PORT_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Dirección del esclavo I2C correspondiente al expansor PCF8574.
 */
#define DEV_ADDRESS 0x27

/**
 * @brief Códigos de estado para las operaciones del puerto PCF8574.
 */
typedef enum
{
    HD44780_PORT_OK = 0,      /**< Operación exitosa */
	HD44780_PORT_ERROR,       /**< Error en la comunicación */
} hd44780_port_status_t;

/**
 * @brief Inicializa la interfaz usada por el HD44780.
 *
 * Esta función puede incluir configuraciones específicas del periférico I2C si fuera necesario.
 * En esta implementación, se asume que la inicialización se realiza externamente mediante la HAL.
 */
hd44780_port_status_t HD44780_Port_Init(void);

/**
 * @brief Realiza una espera bloqueante en milisegundos.
 *
 * @param delay Tiempo de espera en milisegundos.
 */
void HD44780_Port_Delay(uint32_t delay);

/**
 * @brief Envía un nibble (4 bits) al LCD HD44780.
 *
 * @param nibbleWrite Nibble a escribir (en los bits bajos).
 * @param rs Si es true, se interpreta como registro de dato. Si es false, se interpreta como registro
 *           de instrucción.
 *
 * Esta función se utiliza principalmente durante la inicialización y para la escritura en modo 4 bits.
 *
 * @retval HD44780_PORT_OK    La operación fue exitosa.
 * @retval HD44780_PORT_ERROR Hubo un error en la comunicación con el LCD.
 */
hd44780_port_status_t HD44780_Port_Send_Nibble(uint8_t nibbleWrite, bool rs);

/**
 * @brief Escribe un byte completo al LCD (en dos fases de 4 bits).
 *
 * @param byteWrite Byte a escribir.
 * @param rs Si es true, se interpreta como registro de dato. Si es false, se interpreta como registro
 *           de instrucción.
 *
 * El envío se realiza en modo 4 bits, dividiendo el byte en dos nibbles (alto y bajo).
 *
 * @retval HD44780_PORT_OK    La operación fue exitosa.
 * @retval HD44780_PORT_ERROR Hubo un error en la comunicación con el LCD.
 */
hd44780_port_status_t HD44780_Port_Send_Byte(uint8_t byteWrite, bool rs);

#endif /* HD44780_INC_HD44780_PORT_H_ */
