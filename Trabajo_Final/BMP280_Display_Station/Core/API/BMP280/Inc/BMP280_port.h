/**
 * @file BMP280_port.h
 * @author Ing. Lucas Kirschner
 * @date 2 Apr 2025
 * @brief Interfaz de bajo nivel para la comunicación SPI con el sensor BMP280.
 *
 * Este archivo define las funciones de lectura, escritura y transferencia SPI
 * utilizadas para comunicar el sensor BMP280 con un microcontrolador STM32.
 */

#ifndef BMP280_INC_BMP280_PORT_H_
#define BMP280_INC_BMP280_PORT_H_

#include <stdint.h>

#include "stm32f4xx_hal.h" 				/**< Cambiar según el HAL de la plataforma utilizada */

#define SPI3_CS_Pin         GPIO_PIN_2	/**< Pin CS del BMP280, ajustar según hardware */
#define SPI3_CS_GPIO_Port   GPIOD       /**< Puerto CS del BMP280, ajustar según hardware */

extern SPI_HandleTypeDef hspi3;			/**< SPI usado para el BMP280 (ajustar según interfaz usada) */


/**
 * @brief Códigos de estado para las operaciones del puerto BMP280.
 */
typedef enum
{
    BMP280_PORT_OK = 0,      /**< Operación exitosa */
    BMP280_PORT_ERROR,       /**< Error en la comunicación */
} bmp280_port_status_t;

/**
 * @brief Inicializa la interfaz usada por el BMP280.
 *
 * Esta función puede incluir configuraciones específicas del periférico SPI si fuera necesario.
 * En esta implementación, se asume que la inicialización se realiza externamente mediante la HAL.
 */
bmp280_port_status_t BMP280_Port_Init(void);

/**
 * @brief Envía datos al sensor BMP280 vía SPI.
 *
 * @param dataWrite Puntero al buffer de datos a transmitir.
 * @param size Cantidad de bytes a transmitir.
 * @return Estado de la operación (OK, ERROR o TIMEOUT).
 */
bmp280_port_status_t BMP280_Write(uint8_t *dataWrite, uint8_t size);

/**
 * @brief Recibe datos desde el sensor BMP280 vía SPI.
 *
 * @param dataRead Puntero al buffer donde se almacenarán los datos recibidos.
 * @param size Cantidad de bytes a recibir.
 * @return Estado de la operación (OK, ERROR o TIMEOUT).
 */
bmp280_port_status_t BMP280_Read(uint8_t *dataRead, uint8_t size);

/**
 * @brief Realiza una transferencia SPI completa (transmisión + recepción).
 *
 * @param dataWrite Puntero al buffer de datos a transmitir.
 * @param dataRead Puntero al buffer donde se almacenarán los datos recibidos.
 * @param size Cantidad de bytes a transferir.
 * @return Estado de la operación (OK, ERROR o TIMEOUT).
 */
bmp280_port_status_t BMP280_Transfer(uint8_t *dataWrite, uint8_t *dataRead, uint8_t size);

#endif /* BMP280_INC_BMP280_PORT_H_ */
