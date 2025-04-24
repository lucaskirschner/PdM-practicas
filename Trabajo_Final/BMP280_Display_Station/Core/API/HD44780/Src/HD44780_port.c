/**
 * @file HD44780_port.c
 * @author Ing. Lucas Kirschner
 * @date 10 Apr 2025
 * @brief Implementación de las funciones de bajo nivel para controlar un LCD HD44780 mediante I2C.
 *
 * Utiliza la biblioteca HAL de STM32 para la comunicación I2C con el expansor PCF8574,
 * permitiendo el control del display en modo 4 bits mediante software.
 */

#include "HD44780_port.h"

#define DELAY_TIME_1MS  1  	/**< Tiempo base de retardo en milisegundos (1 ms) */
#define I2C_TIMEOUT_MS	50	/**< Timeout para las transmisiones I2C */

/* Máscaras de control */
#define BL_MASK               (1 << 3)   /**< Bit de control de retroiluminación */
#define EN_MASK               (1 << 2)   /**< Bit de control ENABLE para latch de datos */
#define RW_MASK_WRITE         (0 << 1)   /**< RW en 0 → Escritura */
#define RW_MASK_READ          (1 << 1)   /**< RW en 1 → Lectura (no usado en esta implementación) */
#define RS_MASK_IR            (0 << 0)   /**< RS en 0 → Registro de instrucción */
#define RS_MASK_DATA          (1 << 0)   /**< RS en 1 → Registro de datos */

/* Manipulación de nibbles */
#define NIBBLE_SHIFT          4          /**< Corrimiento de nibble para enviarlo por DB7-DB4 */
#define LOW_NIBBLE_MASK       0x0F       /**< Máscara para extraer los 4 bits bajos */
#define HIGH_NIBBLE_MASK      0xF0       /**< Máscara para extraer los 4 bits altos */

/* Dirección del esclavo en escritura y lectura */
#define WRITE_DEV_ADDR ((DEV_ADDRESS << 1) & 0xFE)  /**< Dirección para escritura I2C (bit LSB en 0) */
#define READ_DEV_ADDR  ((DEV_ADDRESS << 1) | 0x01)  /**< Dirección para lectura I2C (bit LSB en 1) */

extern I2C_HandleTypeDef hi2c1; 		 /**< Manejador de la interfaz I2C definida en el proyecto */

hd44780_port_status_t HD44780_Port_Init(void)
{
	  hi2c1.Instance = I2C1;
	  hi2c1.Init.ClockSpeed = 100000;
	  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	  hi2c1.Init.OwnAddress1 = 0;
	  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	  hi2c1.Init.OwnAddress2 = 0;
	  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	  {
	    return HD44780_PORT_ERROR;
	  }

	  return HD44780_PORT_OK;
}

void HD44780_Port_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}

/**
 * @brief Envía un nibble (4 bits) al display HD44780 mediante la expansión I2C del PCF8574.
 *
 * @param nibbleWrite Nibble (4 bits) a enviar.
 * @param rs Indica si se trata de una instrucción (false) o un dato (true).
 *
 * El PCF8574 es un expansor de E/S que comunica 8 bits a través de I2C.
 * Cada bit del PCF8574 se conecta a un pin del controlador HD44780 de la siguiente forma típica:
 *
 *  - P0 -> RS (Register Select)
 *  - P1 -> RW (Read/Write)
 *  - P2 -> EN (Enable)
 *  - P3 -> Retroiluminación (Backlight control)
 *  - P4 -> DB4 (Data Bit 4)
 *  - P5 -> DB5 (Data Bit 5)
 *  - P6 -> DB6 (Data Bit 6)
 *  - P7 -> DB7 (Data Bit 7)
 *
 * Como el HD44780 está configurado en modo de 4 bits, sólo se utilizan las líneas DB7–DB4 para la transferencia de datos.
 *
 * Procedimiento:
 * 1. El nibble (4 bits) a transmitir se coloca en los bits P4–P7 del PCF8574.
 *    Para eso, se realiza un corrimiento de 4 posiciones a la izquierda (`nibbleWrite << 4`).
 * 2. Se combinan las señales de control:
 *    - RS indica si lo transmitido es un comando (RS=0) o datos de usuario (RS=1).
 *    - RW siempre está en 0 (modo escritura).
 *    - EN se coloca en 1 para indicar un flanco activo de habilitación.
 *    - Backlight (BL) se fuerza en 1 para mantener encendida la retroiluminación.
 * 3. Se transmite el byte completo vía I2C al PCF8574.
 * 4. Luego de un pequeño retardo, se baja el bit EN a 0 para generar el flanco de bajada,
 *    lo que "latchea" los datos en el HD44780.
 *
 * Notas:
 * - El proceso de escritura de un byte completo al LCD requiere el envío de dos nibbles consecutivos:
 *   primero el nibble alto y luego el nibble bajo (ver `HD44780_Port_Send_Byte`).
 * - Se utiliza un pequeño retardo de aproximadamente 1 ms entre las operaciones para cumplir con
 *   los requisitos de tiempo de establecimiento y retención del HD44780.
 */
hd44780_port_status_t HD44780_Port_Send_Nibble(uint8_t nibbleWrite, bool rs)
{
    uint8_t data;

    /* Preparación del byte a enviar: datos (DB7–DB4) + señales de control */
    data  = (nibbleWrite << NIBBLE_SHIFT) & HIGH_NIBBLE_MASK;  // Cargar datos en DB7–DB4
    data |= BL_MASK;                                           // Habilitar retroiluminación
    data |= EN_MASK;                                           // Pulso de Enable (activo en alto)
    data |= RW_MASK_WRITE;                                     // Escritura (RW=0)
    data |= (rs ? RS_MASK_DATA : RS_MASK_IR);                  // RS: dato o instrucción

    if(HAL_I2C_Master_Transmit(&hi2c1, WRITE_DEV_ADDR, &data, sizeof(data), I2C_TIMEOUT_MS) != HAL_OK) return HD44780_PORT_ERROR;
    HD44780_Port_Delay(DELAY_TIME_1MS);

    /* Desactivar Enable (flanco de bajada) */
    data &= ~EN_MASK;
    if(HAL_I2C_Master_Transmit(&hi2c1, WRITE_DEV_ADDR, &data, sizeof(data), I2C_TIMEOUT_MS) != HAL_OK) return HD44780_PORT_ERROR;
    HD44780_Port_Delay(DELAY_TIME_1MS);

    return HD44780_PORT_OK;
}

hd44780_port_status_t HD44780_Port_Send_Byte(uint8_t byteWrite, bool rs)
{
    uint8_t upperNibble = (byteWrite >> NIBBLE_SHIFT) & LOW_NIBBLE_MASK;
    uint8_t lowerNibble =  byteWrite &  LOW_NIBBLE_MASK;

    if(HD44780_Port_Send_Nibble(upperNibble, rs) != HD44780_PORT_OK) return HD44780_PORT_ERROR;
    if(HD44780_Port_Send_Nibble(lowerNibble, rs) != HD44780_PORT_OK) return HD44780_PORT_ERROR;

    return HD44780_PORT_OK;
}
