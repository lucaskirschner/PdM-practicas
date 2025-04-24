/**
 * @file HD44780.h
 * @author Ing. Lucas Kirschner
 * @date 10 Apr 2025
 * @brief Definiciones y configuraciones para el controlador LCD HD44780.
 *
 * Este archivo define macros y funciones que permiten el manejo del controlador LCD HD44780.
 *
 * @note Basado en especificaciones del datasheet oficial del HD44780.
 */

#ifndef HD44780_INC_HD44780_H_
#define HD44780_INC_HD44780_H_

#include "HD44780_port.h"

#define ROW_NUMBERS		2
#define COLUMN_NUMBERS	16

/**
 * @defgroup HD44780_IR_Instructions Instrucciones IR del HD44780
 * @{
 */
#define IR_CLEAR_DISPLAY                  0x01 /**< Limpia el display y retorna el cursor a la posición inicial (DDRAM). */
#define IR_RETURN_HOME                    0x02 /**< Retorna el cursor y el display a su posición inicial sin alterar DDRAM. */

/**
 * @brief Genera instrucción de configuración de modo de entrada.
 *
 * @param ID Dirección del incremento:
 *            - 1: Incrementa el cursor.
 *            - 0: Decrementa el cursor.
 * @param S Desplazamiento de display:
 *            - 1: Desplaza el display según ID.
 *            - 0: No desplaza.
 * @return Instrucción para modo de entrada.
 */
#define IR_ENTRY_MODE_SET(ID, S)           (0x04 | ((ID) << 1) | (S))

/**
 * @brief Genera instrucción de control de visualización.
 *
 * @param D Display ON/OFF:
 *            - 1: Encendido.
 *            - 0: Apagado.
 * @param C Cursor ON/OFF:
 *            - 1: Visible.
 *            - 0: Oculto.
 * @param B Blink ON/OFF:
 *            - 1: Cursor parpadeante.
 *            - 0: Sin parpadeo.
 * @return Instrucción de control de display.
 */
#define IR_DISPLAY_CONTROL(D, C, B)        (0x08 | ((D) << 2) | ((C) << 1) | (B))

/**
 * @brief Genera instrucción de desplazamiento de cursor/display.
 *
 * @param SC Selección del desplazamiento:
 *            - 1: Display.
 *            - 0: Cursor.
 * @param RL Dirección:
 *            - 1: Derecha.
 *            - 0: Izquierda.
 * @return Instrucción de desplazamiento.
 */
#define IR_CURSOR_DISPLAY_SHIFT(SC, RL)    (0x10 | ((SC) << 3) | ((RL) << 2))

/**
 * @brief Genera instrucción de configuración de interfaz.
 *
 * @param DL Bus de datos:
 *            - 1: 8 bits.
 *            - 0: 4 bits.
 * @param N Número de líneas:
 *            - 1: 2 líneas.
 *            - 0: 1 línea.
 * @param F Fuente:
 *            - 1: 5x10 puntos.
 *            - 0: 5x8 puntos.
 * @return Instrucción de selección de función.
 */
#define IR_FUNCTION_SET(DL, N, F)           (0x20 | ((DL) << 4) | ((N) << 3) | ((F) << 2))

/**
 * @brief Establece la dirección de memoria CGRAM.
 *
 * @param addr Dirección de CGRAM (6 bits).
 * @return Instrucción para CGRAM.
 */
#define IR_SET_CGRAM_ADDR(addr)             (0x40 | ((addr) & 0x3F))

/**
 * @brief Establece la dirección de memoria DDRAM.
 *
 * @param addr Dirección de DDRAM (7 bits).
 * @return Instrucción para DDRAM.
 */
#define IR_SET_DDRAM_ADDR(addr)              (0x80 | ((addr) & 0x7F))
/** @} */

/** @defgroup HD44780_Options Opciones para configuraciones del HD44780
 * @{
 */

/* Opciones para Entry Mode Set */
#define LCD_ENTRY_INCREMENT                 1 /**< Incrementa dirección tras cada byte escrito. */
#define LCD_ENTRY_DECREMENT                 0 /**< Decrementa dirección tras cada byte escrito. */
#define LCD_ENTRY_SHIFT_ON                  1 /**< Desplaza display al escribir. */
#define LCD_ENTRY_SHIFT_OFF                 0 /**< No desplaza display al escribir. */

/* Opciones para Display Control */
#define LCD_DISPLAY_ON                      1 /**< Display encendido. */
#define LCD_DISPLAY_OFF                     0 /**< Display apagado. */
#define LCD_CURSOR_ON                       1 /**< Cursor visible. */
#define LCD_CURSOR_OFF                      0 /**< Cursor oculto. */
#define LCD_BLINK_ON                        1 /**< Cursor parpadeante. */
#define LCD_BLINK_OFF                       0 /**< Cursor fijo. */

/* Opciones para Cursor/Display Shift */
#define LCD_SHIFT_DISPLAY                   1 /**< Desplaza el display. */
#define LCD_SHIFT_CURSOR                    0 /**< Desplaza el cursor. */
#define LCD_SHIFT_RIGHT                     1 /**< Desplaza hacia la derecha. */
#define LCD_SHIFT_LEFT                      0 /**< Desplaza hacia la izquierda. */

/* Opciones para Function Set */
#define LCD_INTERFACE_8BIT                  1 /**< Interfaz de 8 bits. */
#define LCD_INTERFACE_4BIT                  0 /**< Interfaz de 4 bits. */
#define LCD_DISPLAY_2LINE                   1 /**< Display de 2 líneas. */
#define LCD_DISPLAY_1LINE                   0 /**< Display de 1 línea. */
#define LCD_FONT_5x10                       1 /**< Fuente 5x10 puntos. */
#define LCD_FONT_5x8                        0 /**< Fuente 5x8 puntos. */
/** @} */

/**
 * @brief Estados posibles que pueden devolver las funciones del driver.
 */
typedef enum
{
	HD44780_OK = 0,                 /**< Operación exitosa */
	HD44780_ERROR_PARAM,            /**< Error en el parámetro pasado como argumento */
	HD44780_ERROR_COMM,             /**< Error de comunicación con el display */
} hd44780_status_t;

/**
 * @brief Inicializa el controlador LCD HD44780.
 *
 * Realiza la secuencia de inicialización manual recomendada cuando las condiciones
 * de alimentación no aseguran el correcto funcionamiento del circuito de reset interno.
 *
 * @see HD44780 datasheet, sección "Initializing by Instruction".
 *
 * @retval HD44780_OK           Inicialización exitosa.
 * @retval HD44780_ERROR_COMM   Error de comunicación con el display.
 */
hd44780_status_t HD44780_Init(void);

/**
 * @brief Escribe una cadena de caracteres en el display.
 *
 * @param dataWrite Puntero a la cadena a enviar.
 *
 * @retval HD44780_OK            Escritura exitosa.
 * @retval HD44780_ERROR_PARAM   El puntero a la cadena es NULL.
 * @retval HD44780_ERROR_COMM    Error de comunicación con el display.
 */
hd44780_status_t HD44780_Write(uint8_t *dataWrite);

/**
 * @brief Limpia el contenido del display.
 *
 * @retval HD44780_OK           Limpieza exitosa.
 * @retval HD44780_ERROR_COMM   Error de comunicación con el display.
 */
hd44780_status_t HD44780_Clear(void);

/**
 * @brief Posiciona el cursor en una ubicación específica.
 *
 * @param row Fila (1 o 2).
 * @param column Columna (1–16).
 *
 * @retval HD44780_OK            Posicionamiento exitoso.
 * @retval HD44780_ERROR_PARAM   Parámetro de fila o columna fuera de rango.
 * @retval HD44780_ERROR_COMM    Error de comunicación con el display.
 */
hd44780_status_t HD44780_Set_Cursor(uint8_t row, uint8_t column);

/**
 * @brief Escribe un valor entero en el display.
 *
 * @param value Valor entero a mostrar.
 *
 * @retval HD44780_OK           Escritura exitosa.
 * @retval HD44780_ERROR_COMM   Error de comunicación con el display.
 */
hd44780_status_t HD44780_Write_int(int16_t value);

#endif /* HD44780_INC_HD44780_H_ */
