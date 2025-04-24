/**
 * @file BMP280.h
 * @author Ing. Lucas Kirschner
 * @date 2 Apr 2025
 * @brief Driver de alto nivel para el sensor BMP280 (temperatura y presión).
 *
 * Este archivo contiene las definiciones públicas necesarias para interactuar con
 * el sensor BMP280. Provee funciones para inicializar el sensor, configurar su
 * funcionamiento y leer datos compensados de temperatura y presión.
 */

#ifndef BMP280_INC_BMP280_H_
#define BMP280_INC_BMP280_H_

#include <stdint.h>
#include <stdbool.h>

#include "BMP280_port.h"

/* BM280 Memory map --------------------------------------------*/
#define REG_TEMP_XLSB			((uint8_t) 0xFC)
#define REG_TEMP_LSB			((uint8_t) 0xFB)
#define REG_TEMP_MSB			((uint8_t) 0xFA)
#define REG_PRESS_XLSB			((uint8_t) 0xF9)
#define REG_PRESS_LSB			((uint8_t) 0xF8)
#define REG_PRESS_MSB			((uint8_t) 0xF7)
#define REG_CONFIG				((uint8_t) 0xF5)
#define REG_CTRL_MEAS			((uint8_t) 0xF4)
#define REG_STATUS				((uint8_t) 0xF3)
#define REG_RESET				((uint8_t) 0xE0)
#define REG_ID					((uint8_t) 0xD0)
#define REG_CALIB_START			((uint8_t) 0x88)

/* BM280 Config register ---------------------------------------*/
#define CONFIG_T_SB				((uint8_t) 0xE0)
#define CONFIG_FILTER			((uint8_t) 0x1C)
#define CONFIG_SPI3W_EN			((uint8_t) 0x01)

/* BM280 Control meas register ---------------------------------*/
#define CTRL_MEAS_OSRS_T		((uint8_t) 0xE0)
#define CTRL_MEAS_OSRS_P		((uint8_t) 0x1C)
#define CTRL_MEAS_MODE			((uint8_t) 0x03)

/* BM280 Status register ---------------------------------------*/
#define STATUS_MEASURING		((uint8_t) 0x08)
#define STATUS_IM_UPDATE		((uint8_t) 0x01)

/* BM280 Standby times -----------------------------------------*/
#define STANDBY_05				((uint8_t) 0x00)
#define STANDBY_625				((uint8_t) 0x20)
#define STANDBY_125				((uint8_t) 0x40)
#define STANDBY_250				((uint8_t) 0x60)
#define STANDBY_500				((uint8_t) 0x80)
#define STANDBY_1000			((uint8_t) 0xA0)
#define STANDBY_2000			((uint8_t) 0xC0)
#define STANDBY_4000			((uint8_t) 0xE0)

/* BM280 Filter coefficient ------------------------------------*/
#define FILTER_COEFF_OFF		((uint8_t) 0x00)
#define FILTER_COEFF_X2			((uint8_t) 0x04)
#define FILTER_COEFF_X4			((uint8_t) 0x08)
#define FILTER_COEFF_X8			((uint8_t) 0x0c)
#define FILTER_COEFF_X16		((uint8_t) 0x10)

/* BM280 Temperature oversampling settings ---------------------*/
#define TEMP_OVER_SKIP			((uint8_t) 0x00)
#define TEMP_OVER_x1			((uint8_t) 0x20)
#define TEMP_OVER_x2			((uint8_t) 0x40)
#define TEMP_OVER_x4			((uint8_t) 0x60)
#define TEMP_OVER_x8			((uint8_t) 0x80)
#define TEMP_OVER_x16			((uint8_t) 0xF0)

/* BM280 Pressure oversampling settings ------------------------*/
#define PRESS_OVER_SKIP			((uint8_t) 0x00)
#define PRESS_OVER_x1			((uint8_t) 0x04)
#define PRESS_OVER_x2			((uint8_t) 0x08)
#define PRESS_OVER_x4			((uint8_t) 0x0C)
#define PRESS_OVER_x8			((uint8_t) 0x10)
#define PRESS_OVER_x16			((uint8_t) 0x14)

/* BM280 Modes -------------------------------------------------*/
#define SLEEP_MODE				((uint8_t) 0x00)
#define FORCED_MODE				((uint8_t) 0x01)
#define NORMAL_MODE				((uint8_t) 0x03)

/* BM280 SPI Read adn Write masks ------------------------------*/
#define READ_MASK				((uint8_t) 0x80)
#define WRITE_MASK				((uint8_t) 0x7F)

/* BM280 packages ----------------------------------------------*/
#define CHIP_ID					((uint8_t) 0x58)
#define CHIP_RESET				((uint8_t) 0xB6)
#define DUMMY_PKG				((uint8_t) 0x00)

/**
 * @brief Estados posibles que pueden devolver las funciones del driver.
 */
typedef enum
{
	BMP280_OK = 0,                 /**< Operación exitosa */
	BMP280_DATA_RDY,               /**< Nuevos datos disponibles */
	BMP280_DATA_NOT_RDY,           /**< Datos aún no listos */
	BMP280_ERROR_PARAM,            /**< Error en el parámetro pasado como argumento */
	BMP280_ERROR_COMM,             /**< Error de comunicación con el sensor */
	BMP280_ERROR_INVALID_ID,       /**< ID del sensor no coincide con BMP280 */
	BMP280_ERROR_INVALID_MODE,     /**< Modo no válido */
	BMP280_ERROR_NAN               /**< Resultado no numérico */
} bmp280_status_t;

/**
 * @brief Estructura de configuración y datos del sensor BMP280.
 */
typedef struct
{
	/* Últimas lecturas compensadas */
	float temperature;             /**< Temperatura en grados Celsius */
	float pressure;                /**< Presión en Pascales */
} bmp280_t;

/**
 * @brief Inicializa el sensor BMP280.
 *
 * Esta función verifica la conexión con el sensor, lee su ID y aplica
 * la configuración almacenada en la estructura `bmp280_t`.
 *
 * @param dev Puntero a la estructura de configuración del sensor.
 * @return Estado de la operación (BMP280_OK si fue exitosa).
 */
bmp280_status_t BMP280_Init(void);

/**
 * @brief Dispara una medición en modo "forced".
 *
 * Esta función inicia una nueva medición si el sensor está en modo forzado.
 *
 * @param dev Puntero a la estructura del sensor.
 * @return Estado de la operación.
 */
bmp280_status_t	BMP280_Trigger_Measurement(void);

/**
 * @brief Verifica si el sensor sigue midiendo.
 *
 * Útil para saber si ya se puede leer el resultado luego de usar `BMP280_Trigger_Measurement()`.
 *
 * @param dev Puntero a la estructura del sensor.
 * @return BMP280_DATA_RDY si los datos están listos, BMP280_DATA_NOT_RDY si no.
 */
bmp280_status_t BMP280_Is_Measuring(void);

/**
 * @brief Lee y compensa los datos de presión y temperatura.
 *
 * Esta función realiza una lectura de los registros crudos y aplica la compensación
 * utilizando los coeficientes internos del sensor.
 *
 * @param dev Puntero a la estructura del sensor. Al finalizar, contiene los nuevos datos.
 * @return Estado de la operación.
 */
bmp280_status_t BMP280_Update_Parameters(bmp280_t* dev);

#endif /* BMP280_INC_BMP280_H_ */
