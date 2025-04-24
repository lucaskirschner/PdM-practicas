/**
 * @file BMP280.c
 * @author Ing. Lucas Kirschner
 * @date 2 Apr 2025
 * @brief Implementación del driver para el sensor BMP280 mediante comunicación SPI.
 *
 * Esta implementación incluye funciones para inicializar el sensor, disparar mediciones,
 * leer datos crudos y aplicar compensación según las constantes de calibración internas.
 * Se presta especial atención al correcto manejo del protocolo SPI, los modos de medición,
 * y la lectura en ráfaga de registros, de acuerdo a lo especificado por la hoja de datos del BMP280.
 */

#include "BMP280.h"
#include "API_swo.h"

/* Estructura privada para almacenar las constantes de calibración internas del BMP280 */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_data_t;

static bmp280_calib_data_t calib;

/**
 * @brief Lee los coeficientes de calibración desde los registros del BMP280.
 *
 * Los coeficientes de calibración están almacenados en registros contiguos entre 0x88 y 0x9F.
 * Se realiza una lectura por ráfaga de 24 bytes + 1 byte de dirección (25 en total),
 * optimizando así la comunicación SPI.
 *
 * @return BMP280_OK si se leyeron correctamente, BMP280_ERROR_COMM si hubo un error de comunicación.
 */
static bmp280_status_t BMP280_Get_Calibration(void)
{
	uint8_t txBuffer[25];
	uint8_t rxBuffer[25];

	txBuffer[0] = REG_CALIB_START | READ_MASK;
	for(uint8_t i = 1; i < 25; i++)
		txBuffer[i] = 0x00;

	if(BMP280_Transfer(txBuffer, rxBuffer, 25) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;
																			/* LSB  /  MSB	*/
	calib.dig_T1 = (uint16_t)(rxBuffer[ 1] 	| (rxBuffer[ 2] << 8));			/* 0x88 / 0x89	*/
	calib.dig_T2 = (int16_t) (rxBuffer[ 3] 	| (rxBuffer [4] << 8));
	calib.dig_T3 = (int16_t) (rxBuffer[ 5] 	| (rxBuffer[ 6] << 8));
	calib.dig_P1 = (uint16_t)(rxBuffer[ 7] 	| (rxBuffer[ 8] << 8));
	calib.dig_P2 = (int16_t) (rxBuffer[ 9] 	| (rxBuffer[10] << 8));
	calib.dig_P3 = (int16_t) (rxBuffer[11]	| (rxBuffer[12] << 8));
	calib.dig_P4 = (int16_t) (rxBuffer[13] 	| (rxBuffer[14] << 8));
	calib.dig_P5 = (int16_t) (rxBuffer[15] 	| (rxBuffer[16] << 8));
	calib.dig_P6 = (int16_t) (rxBuffer[17] 	| (rxBuffer[18] << 8));
	calib.dig_P7 = (int16_t) (rxBuffer[19] 	| (rxBuffer[20] << 8));
	calib.dig_P8 = (int16_t) (rxBuffer[21] 	| (rxBuffer[22] << 8));
	calib.dig_P9 = (int16_t) (rxBuffer[23] 	| (rxBuffer[24] << 8));			/* 0x9E / 0x9F	*/

	return BMP280_OK;
}

/**
 * @brief Lee los valores crudos de temperatura y presión desde los registros del sensor.
 *
 * Realiza una lectura por ráfaga de los 6 bytes correspondientes (3 bytes por parámetro),
 * comenzando desde REG_PRESS_MSB. El BMP280 recomienda esta lectura continua para garantizar
 * la coherencia entre ambos parámetros.
 *
 * @param dev Puntero al descriptor del dispositivo BMP280.
 * @param rawTemp Puntero donde se almacenará la temperatura cruda (20 bits).
 * @param rawPress Puntero donde se almacenará la presión cruda (20 bits).
 * @return BMP280_OK si fue exitoso, o un código de error en caso contrario.
 */
static bmp280_status_t BMP280_Read_Raw_Parameters(bmp280_t *dev, uint32_t *rawTemp, uint32_t *rawPress)
{
	if (dev == NULL) return BMP280_ERROR_PARAM;

	uint8_t txBuffer[7];
	uint8_t rxBuffer[7];

	txBuffer[0] = REG_PRESS_MSB | READ_MASK;
	for(uint8_t i = 1; i < 7; i++)
		txBuffer[i] = 0x00;

	if(BMP280_Transfer(txBuffer, rxBuffer, 7) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	// Presión: 20 bits (MSB << 12) | (LSB << 4) | (XLSB >> 4)
	*rawPress =	((int32_t)rxBuffer[1] << 12) |
	            ((int32_t)rxBuffer[2] << 4)  |
	            ((int32_t)rxBuffer[3] >> 4);

	// Temperatura: 20 bits (MSB << 12) | (LSB << 4) | (XLSB >> 4)
	*rawTemp = 	((int32_t)rxBuffer[4] << 12) |
	            ((int32_t)rxBuffer[5] << 4)  |
	            ((int32_t)rxBuffer[6] >> 4);

	return BMP280_OK;

}

/**
 * @brief Compensa los valores de temperatura y presión crudos del sensor BMP280.
 *
 * Esta función aplica las fórmulas de compensación definidas en la hoja de datos del BMP280
 * para convertir los valores crudos de temperatura y presión en valores reales en unidades
 * físicas. Los resultados se almacenan en el struct del dispositivo.
 *
 * @param dev      Puntero a la estructura `bmp280_t` que contiene los valores compensados.
 * @param rawTemp      Valor crudo de temperatura leído del sensor.
 * @param rawPress     Valor crudo de presión leído del sensor.
 *
 * @return bmp280_status_t Devuelve `BMP280_OK` si la compensación fue exitosa, o `BMP280_ERROR_NAN`
 *                         si ocurrió una división por cero durante la compensación de presión.
 *
 * @note Esta función utiliza coeficientes de calibración globales almacenados en `calib`.
 *       Se recomienda asegurarse de que dichos coeficientes hayan sido leídos correctamente
 *       desde el sensor antes de ejecutar esta función.
 *
 * @warning Esta función imprime por consola los valores de temperatura y presión compensados.
 *          En sistemas embebidos sin salida estándar, esta funcionalidad debe desactivarse o redirigirse.
 */
static bmp280_status_t BMP280_Compensate_Values(bmp280_t *dev, uint32_t rawTemp, uint32_t rawPress)
{
	float var1, var2, t_fine;
	/* Compensación de tempertaura */

	var1 = (((float)rawTemp) / 16384.0 - ((float)calib.dig_T1) / 1024.0) * ((float)calib.dig_T2);
	var2 = ((((float)rawTemp) / 131072.0 - ((float)calib.dig_T1) / 8192.0) *
	        (((float)rawTemp) / 131072.0 - ((float)calib.dig_T1) / 8192.0)) * ((float)calib.dig_T3);

	t_fine = (int32_t)(var1 + var2);
	dev->temperature = (var1 + var2) / 5120.0;

	printf("Temperature: %0.1f C\r\n",dev->temperature);

	/* Compensación de presión */

    var1 = ((float)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((float)calib.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((float)calib.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((float)calib.dig_P4) * 65536.0);
    var1 = (((float)calib.dig_P3) * var1 * var1 / 524288.0 +
            ((float)calib.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((float)calib.dig_P1);

    if (var1 == 0.0) return BMP280_ERROR_NAN;

    dev->pressure = 1048576.0 - (float)rawPress;
    dev->pressure = (dev->pressure - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((float)calib.dig_P9) * dev->pressure * dev->pressure / 2147483648.0;
    var2 = dev->pressure * ((float)calib.dig_P8) / 32768.0;
    dev->pressure = dev->pressure + (var1 + var2 + ((float)calib.dig_P7)) / 16.0;
    dev->pressure /= 100.0;

    printf("Pressure: %0.1f hPa\r\n",dev->pressure);

    return BMP280_OK;
}

/**
 * @brief Inicializa el sensor BMP280.
 *
 * Se realiza un envío de paquete dummy para estabilizar la línea SCK (recomendado para SPI),
 * luego se lee el ID del sensor y se configuran los registros `config` y `ctrl_meas`.
 * Finalmente, se cargan los coeficientes de calibración. Es importante que la función
 * sea llamada una vez tras energizar el sensor.
 *
 * @param dev Puntero a la estructura del dispositivo a inicializar.
 * @return BMP280_OK si se inicializó correctamente, o un código de error.
 */
bmp280_status_t BMP280_Init(void)
{
	uint8_t txBuffer[2];
	uint8_t rxBuffer[2];

	/* Inicializo el periférico SPI*/
	if(BMP280_Port_Init() != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	/* Envio dato dummy para estabilizar la linea SCK		*/
	txBuffer[0] = DUMMY_PKG;
	if(BMP280_Write(txBuffer, 1) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	/* Verifico leyendo id del bmp280						*/
	txBuffer[0] = REG_ID | READ_MASK;
	txBuffer[1] = DUMMY_PKG;
	if(BMP280_Transfer(txBuffer, rxBuffer, 2) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	if(rxBuffer[1] != CHIP_ID)
		return BMP280_ERROR_INVALID_ID;

	/* Escribo configuración en el registro config			*/
	txBuffer[0] = REG_CONFIG & WRITE_MASK;
	txBuffer[1] = STANDBY_05 | FILTER_COEFF_OFF;
	if(BMP280_Write(txBuffer, 2) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	/* Escribo configuración en el registro ctrl_meas		*/
	txBuffer[0] = REG_CTRL_MEAS & WRITE_MASK;
	txBuffer[1] = TEMP_OVER_x1 | PRESS_OVER_x1 | FORCED_MODE;
	if(BMP280_Write(txBuffer, 2) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	if(BMP280_Get_Calibration() != BMP280_OK)
		return BMP280_ERROR_COMM;

	return BMP280_OK;
}

/**
 * @brief Dispara una medición en modo FORCED.
 *
 * En este modo, el sensor permanece dormido y solo se activa cuando se escribe en
 * `ctrl_meas`. Una vez finalizada la medición, vuelve automáticamente a SLEEP.
 *
 * @param dev Puntero a la estructura del dispositivo.
 * @return BMP280_OK si el disparo fue exitoso, o un error si el modo no es FORCED.
 */
bmp280_status_t	BMP280_Trigger_Measurement(void)
{
	uint8_t txBuffer[2];

	/* Paso a modo forced escribiendo  en ctrl_meas			*/
	txBuffer[0] = REG_CTRL_MEAS & WRITE_MASK;
	txBuffer[1] = TEMP_OVER_x1 | PRESS_OVER_x1 | FORCED_MODE;
	if(BMP280_Write(txBuffer, 2) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	return BMP280_OK;
}

/**
 * @brief Verifica si el sensor aún se encuentra realizando una medición.
 *
 * Esta función consulta el bit de estado `measuring` del registro STATUS (0xF3).
 *
 * @param dev Puntero al dispositivo BMP280.
 * @return BMP280_DATA_RDY si los datos están listos, BMP280_DATA_NOT_RDY si aún mide,
 *         o un código de error si la lectura falla.
 */
bmp280_status_t BMP280_Is_Measuring(void)
{
	uint8_t txBuffer[2];
	uint8_t rxBuffer[2];

	txBuffer[0] = REG_STATUS | READ_MASK;
	txBuffer[1] = DUMMY_PKG;

	if (BMP280_Transfer(txBuffer, rxBuffer, 2) != BMP280_PORT_OK)
		return BMP280_ERROR_COMM;

	if(rxBuffer[1] & STATUS_MEASURING)
		return BMP280_DATA_NOT_RDY;
	else
		return BMP280_DATA_RDY;
}

/**
 * @brief Actualiza los valores de temperatura y presión del sensor.
 *
 * Aplica las fórmulas de compensación flotante detalladas en la hoja de datos, que utilizan
 * los coeficientes internos previamente cargados. La lectura se basa en los registros
 * de datos crudos, y el orden de las operaciones busca preservar la precisión final.
 *
 * @param dev Puntero al descriptor del BMP280. La estructura se actualiza con los valores.
 * @return BMP280_OK si se actualizaron correctamente, o un error correspondiente.
 */
bmp280_status_t BMP280_Update_Parameters(bmp280_t* dev)
{
	if (dev == NULL)
		return BMP280_ERROR_PARAM;

	uint32_t rawTemp, rawPress;
	bmp280_status_t status;

	status = BMP280_Read_Raw_Parameters(dev, &rawTemp, &rawPress);
	if(status != BMP280_OK)
		return status;

	if(rawTemp == 0 || rawPress == 0)
		return BMP280_ERROR_COMM;		/* Si los datos leídos son cero, indica un posible error en la linea MISO */

	status = BMP280_Compensate_Values(dev, rawTemp, rawPress);
	if(status != BMP280_OK)
		return status;

	return BMP280_OK;
}
