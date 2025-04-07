/**
 * @file API_debounce.h
 * @brief API para gestión de botones con lógica de antirrebote mediante una máquina de estados finita (FSM).
 * @author Lucas Kirschner
 * @date 27 Mar 2025
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include <stdint.h>
#include <stdbool.h>
#include "API_delay.h"
#include "stm32f4xx_hal.h"

typedef bool bool_t;

/**
 * @brief Estados posibles de la FSM para el manejo del botón.
 */
typedef enum {
	BUTTON_UP,       /**< Botón sin presionar */
	BUTTON_FALLING,  /**< Posible detección de pulsación */
	BUTTON_DOWN,     /**< Botón presionado */
	BUTTON_RISING    /**< Posible detección de liberación */
} debounceState_t;

/**
 * @brief Estructura que representa un botón con su estado y configuración.
 */
typedef struct {
	GPIO_TypeDef* port;       /**< Puerto GPIO del botón */
	uint16_t pin;             /**< Pin del botón */
	debounceState_t state;    /**< Estado actual de la FSM */
	delay_t delay;            /**< Retardo software para antirrebote */
	bool_t fallingFlag;       /**< Flag de pulsación detectada */
} debounce_t;

/**
 * @brief Inicializa la FSM de antirrebote para un botón determinado.
 *
 * @param btn Puntero a la estructura debounce_t a inicializar.
 * @param port Puerto GPIO al que está conectado el botón.
 * @param pin Pin del GPIO utilizado por el botón.
 */
void debounceFSM_init(debounce_t* btn, GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief Actualiza el estado de la FSM de antirrebote. Debe llamarse periódicamente.
 *
 * @param btn Puntero a la estructura debounce_t a actualizar.
 */
debounceState_t debounceFSM_update(debounce_t* btn);

/**
 * @brief Consulta si ocurrió una pulsación del botón (flanco descendente).
 *
 * @param btn Puntero a la estructura debounce_t asociada al botón.
 * @return true si se detectó una pulsación, false en caso contrario.
 */
bool_t readKey(debounce_t* btn);

#endif /* API_INC_API_DEBOUNCE_H_ */
