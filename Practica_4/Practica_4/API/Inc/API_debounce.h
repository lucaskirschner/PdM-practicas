/*
 * API_debounce.h
 *
 *  Created on: Mar 27, 2025
 *      Author: Lucas Kirschner
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;

void debounceFSM_init(void);		// carga el estado inicial de la FSM
void debounceFSM_update(void);		// Resuelve la lógica de la FSM
bool_t readKey(void);				// Retorna true si ocurrio una detección

#endif /* API_INC_API_DEBOUNCE_H_ */
