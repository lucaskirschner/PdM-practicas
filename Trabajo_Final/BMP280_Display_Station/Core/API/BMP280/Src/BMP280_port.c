/**
 * @file BMP280_port.c
 * @author Ing. Lucas Kirschner
 * @date 2 Apr 2025
 * @brief Implementación de la interfaz SPI de bajo nivel para el sensor BMP280.
 *
 * Este archivo implementa las funciones de lectura, escritura y transferencia SPI
 * que permiten la comunicación entre un microcontrolador STM32 y el sensor BMP280.
 */

#include "BMP280_port.h"

#define SPI_TIMEOUT_MS	50	/**< Timeout para las transmisiones SPI */

/**
 * @brief Activa el pin Chip Select (CS) del sensor BMP280.
 *
 * Establece el pin CS en nivel bajo, iniciando una comunicación SPI.
 */
static void BMP280_CS_Enable(void)
{
    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
}

/**
 * @brief Desactiva el pin Chip Select (CS) del sensor BMP280.
 *
 * Establece el pin CS en nivel alto, finalizando una comunicación SPI.
 * El flanco ascendente de este pin marca el fin de una lectura por ráfaga.
 */
static void BMP280_CS_Disable(void)
{
    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief Convierte un estado de HAL a un código de estado del puerto BMP280.
 *
 * @param hal_status Estado devuelto por las funciones de la HAL.
 * @return Código de estado correspondiente de tipo ::bmp280_port_status_t.
 */
static bmp280_port_status_t BMP280_ConvertStatus(HAL_StatusTypeDef hal_status)
{
    switch (hal_status)
    {
        case HAL_OK:
            return BMP280_PORT_OK;
        default:
            return BMP280_PORT_ERROR;
    }
}

bmp280_port_status_t BMP280_Port_Init(void)
{
	  hspi3.Instance = SPI3;
	  hspi3.Init.Mode = SPI_MODE_MASTER;
	  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
	  hspi3.Init.NSS = SPI_NSS_SOFT;
	  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi3.Init.CRCPolynomial = 10;
	  if (HAL_SPI_Init(&hspi3) != HAL_OK)
	  {
	    return BMP280_PORT_ERROR;
	  }

	  return BMP280_PORT_OK;
}

bmp280_port_status_t BMP280_Write(uint8_t *dataWrite, uint8_t size)
{
    if (dataWrite == NULL || size == 0)
        return BMP280_PORT_ERROR;

    BMP280_CS_Enable();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi3, dataWrite, size, SPI_TIMEOUT_MS);
    BMP280_CS_Disable();

    return BMP280_ConvertStatus(status);
}

bmp280_port_status_t BMP280_Read(uint8_t *dataRead, uint8_t size)
{
    if (dataRead == NULL || size == 0)
        return BMP280_PORT_ERROR;

    BMP280_CS_Enable();
    HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi3, dataRead, size, SPI_TIMEOUT_MS);
    BMP280_CS_Disable();

    return BMP280_ConvertStatus(status);
}

bmp280_port_status_t BMP280_Transfer(uint8_t *dataWrite, uint8_t *dataRead, uint8_t size)
{
    if (dataWrite == NULL || dataRead == NULL || size == 0)
        return BMP280_PORT_ERROR;

    BMP280_CS_Enable();
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi3, dataWrite, dataRead, size, SPI_TIMEOUT_MS);
    BMP280_CS_Disable();

    return BMP280_ConvertStatus(status);
}
