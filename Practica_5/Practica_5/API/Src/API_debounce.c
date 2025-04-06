/*
 * API_debounce.c
 *
 *  Created on: Mar 27, 2025
 *      Author: Lucas Kirschner
 */

#include "API_debounce.h"
#include "API_delay.h"
#include "API_swo.h"
#include "stm32f4xx_hal.h"

#define DEBOUNCE_DELAY 40				// Delay del antirrebote por software

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC

typedef enum{							// Estados de la FSM
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING,
} debounceState_t;

static delay_t delay;
static debounceState_t debounce;
static bool_t fallingFlag;

static void buttonPressed(void);
static void buttonReleased(void);
static bool_t buttonState(void);
static void Error_Handler(void);


/* debounceFSM_init: Establece estado inicial de la máquina de estados y delay del retardo por software.
 * se llama una vez al inicio o al ocurrir un default por estado inconsistente.
 * Parametros:
 *  void
 * Retorna:
 * 	void
 */
void debounceFSM_init(void)
{
	delayInit(&delay,DEBOUNCE_DELAY);	// Establece delay de la FSM
	debounce = BUTTON_UP;				// Estado inicial de la FSM
}


/* debounceFSM_update: Controla toda la lógica de la FSM. Se llama en cada iteración en el super-loop.
 * Parametros:
 *  void
 * Retorna:
 * 	void
 */
void debounceFSM_update(void)
{
	switch (debounce)
	{
		case BUTTON_UP:
			if(buttonState())									// Si se detecto un cambio
			{
				delayRead(&delay);								// Inicio el retardo
				debounce = BUTTON_FALLING;						// Paso a modo falling
				printf("BUTTON_UP -> BUTTON_FALLING\n");		// Salida SWO para debbuging
			}
			break;
		case BUTTON_FALLING:
			if(delayRead(&delay))								// Si se cumplió el tiempo de retardo
			{
				if(buttonState())								// Vuelvo a preguntar por el estado del botón
				{
					buttonPressed(); 							// Hago tarea al pulsar el botón
					debounce = BUTTON_DOWN;						// Paso al estado down
					printf("BUTTON_FALLING -> BUTTON_DOWN\n");	// Salida SWO para debbuging
				}
				else											// Si no sigue pulsado
				{
					debounce = BUTTON_UP;						// Vuelvo al estado up
					printf("BUTTON_FALLING -> BUTTON_UP\n");	// Salida SWO para debbuging
				}
			}
			break;
		case BUTTON_DOWN:
			if(!buttonState())									// Si se detecto un cambio
			{
				delayRead(&delay);								// Inicio el retardo
				debounce = BUTTON_RISING;						// Paso a modo rissing
				printf("BUTTON_DOWN -> BUTTON_RISING\n");		// Salida SWO para debbuging
			}
			break;
		case BUTTON_RISING:
			if(delayRead(&delay))								// Si se cumplió el tiempo de retardo
			{
				if(!buttonState())								// Vuelvo a preguntar por el estado del botón
				{
					buttonReleased(); 							// Hago tarea al soltar el botón
					debounce = BUTTON_UP;						// Paso al estado up
					printf("BUTTON_RISING -> BUTTON_UP\n");		// Salida SWO para debbuging
				}
				else											// Si no sigue pulsado
				{
					debounce = BUTTON_DOWN;						// Vuelvo al estado down
					printf("BUTTON_RISING -> BUTTON_DOWN\n");	// Salida SWO para debbuging
				}
			}
			break;
		default: Error_Handler();								// En otro caso, acuso error fatal
	};
}


/* buttonPressed: realiza las tareas correspondientes al pulsar el botón, en este caso, poner un flag interno en true.
 * Parametros:
 *  void
 * Retorna:
 * 	void
 */
static void buttonPressed(void)
{
	fallingFlag = true;
}

/* buttonReleased: realiza las tareas correspondientes al soltar el botón, en este caso, no hace nada.
 * Parametros:
 *  void
 * Retorna:
 * 	void
 */
static void buttonReleased(void)
{
	/* No se hace nada en este caso */
}


/* readKey: Retorna true si se ha detectado una pulsación del botón y restablece el flag intenro indicativo
 * Parametros:
 *  void
 * Retorna:
 * 	bool_t:
 * 		true: si ocurrio una pulsación
 * 		false: si no ocurrio la pulsación
 */
bool_t readKey(void)
{
	if(fallingFlag)
	{
		fallingFlag = false;
		return true;
	}
	else
	{
		return false;
	}
}

/* buttonState: Retorna estado del boton de usuario
 * Parametros:
 * 	void
 * Retorna:
 * 	bool_t:
 * 		true: botón pulsado
 * 		false: botón sin pulsar
 */
static bool_t buttonState(void)
{
	return !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
}

/* Error_Handler: Handler que se ejecuta en caso de error fatal
 * Parametros:
 * 	void
 * Retorna:
 * 	void
 */
static void Error_Handler(void)
{
	  __disable_irq();
	  while (1){}
}

