/**
 * @file API_debounce.c
 * @author Ing. Lucas Kirschner
 * @date 27 Mar 2025
 * @brief API para gestión de botones con lógica de antirrebote mediante una máquina de estados finita (FSM).
 */

#include "API_debounce.h"
#include "API_swo.h"

#define DEBOUNCE_DELAY 40

/**
 * @brief Callback privado que se ejecuta al detectar una pulsación válida.
 *
 * @param btn Puntero a la estructura debounce_t correspondiente.
 */
static void buttonPressed(debounce_t* btn)
{
	btn->fallingFlag = true;
}

/**
 * @brief Callback privado que se ejecuta al detectar la liberación del botón válida.
 *
 * @param btn Puntero a la estructura debounce_t correspondiente.
 */
static void buttonReleased(debounce_t* btn)
{
	btn->risingFlag = true;
}

/**
 * @brief Lee el estado lógico del pin del botón (invertido).
 *
 * @param btn Puntero a la estructura debounce_t correspondiente.
 * @return true si el botón está presionado, false si está liberado.
 */
static bool_t buttonState(debounce_t* btn)
{
	return !HAL_GPIO_ReadPin(btn->port, btn->pin);
}

/**
 * @brief Manejo de error en caso de estado inválido.
 */
static void Error_Handler(void)
{
	__disable_irq();
	while (1) {}
}

void debounceFSM_init(debounce_t* btn, GPIO_TypeDef* port, uint16_t pin)
{
	btn->port = port;
	btn->pin = pin;
	btn->state = BUTTON_UP;
	btn->fallingFlag = false;
	delayInit(&btn->delay, DEBOUNCE_DELAY);
}

debounceState_t debounceFSM_update(debounce_t* btn)
{
	switch (btn->state)
	{
		case BUTTON_UP:
			if (buttonState(btn)) 								// Si se detecta un posible flanco descendente
			{
				delayRead(&btn->delay); 						// Se inicia el retardo de confirmación
				btn->state = BUTTON_FALLING; 					// Se pasa al estado de validación de pulsación
				printf("BUTTON_UP -> BUTTON_FALLING\n"); 		// Salida por consola para debug
			}
			return BUTTON_UP;
			break;

		case BUTTON_FALLING:
			if (delayRead(&btn->delay)) 						// Si se cumplió el tiempo de retardo
			{
				if (buttonState(btn)) 							// Se confirma que el botón sigue presionado
				{
					buttonPressed(btn); 						// Acción asociada al evento de pulsación
					btn->state = BUTTON_DOWN; 					// Se pasa al estado presionado
					printf("BUTTON_FALLING -> BUTTON_DOWN\n");

					return BUTTON_FALLING;						// Informo que se produjo un flanco descendente valido
				}
				else 											// Se descartó el flanco por rebote
				{
					btn->state = BUTTON_UP;					 	// Se vuelve al estado sin presionar
					printf("BUTTON_FALLING -> BUTTON_UP\n");

					return BUTTON_UP;
				}
			}
			break;

		case BUTTON_DOWN:
			if (!buttonState(btn)) 								// Si se detecta un posible flanco ascendente
			{
				delayRead(&btn->delay); 						// Se inicia el retardo de confirmación
				btn->state = BUTTON_RISING; 					// Se pasa al estado de validación de liberación
				printf("BUTTON_DOWN -> BUTTON_RISING\n");
			}
			return BUTTON_DOWN;
			break;

		case BUTTON_RISING:
			if (delayRead(&btn->delay)) 						// Si se cumplió el tiempo de retardo
			{
				if (!buttonState(btn)) 							// Se confirma que el botón está liberado
				{
					buttonReleased(btn); 						// Acción asociada al evento de liberación
					btn->state = BUTTON_UP; 					// Se vuelve al estado sin presionar
					printf("BUTTON_RISING -> BUTTON_UP\n");

					return BUTTON_RISING;						// Informo que se produjo un flanco ascendente valido
				}
				else 											// Se descartó el flanco por rebote
				{
					btn->state = BUTTON_DOWN;					// Se vuelve al estado presionado
					printf("BUTTON_RISING -> BUTTON_DOWN\n");

					return BUTTON_DOWN;
				}
			}
			break;

		default:
			Error_Handler(); 									// Manejo de error por estado inválido
	}

	return btn->state;											// Nunca llegará a esta condición
}

bool_t readKeyPressed(debounce_t* btn)
{
	if (btn->fallingFlag)
	{
		btn->fallingFlag = false;
		return true;
	}
	return false;
}

bool_t readKeyReleased(debounce_t* btn)
{
	if (btn->risingFlag)
	{
		btn->risingFlag = false;
		return true;
	}
	return false;
}

