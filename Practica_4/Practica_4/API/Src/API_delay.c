/*
 * API_delay.c
 *
 *  Created on: Mar 20, 2025
 *      Author: Lucas Kirschner
 */

#include "API_delay.h"
#include "stm32f4xx_hal.h"

static void Error_Handler(void);

/* delayInit: Iniciliza y establece la duración del delay no bloqueante
 * Parametros:
 * 	delay_t * delay: puntero a estructura con configuración del delay
 * 	tick_t duration: duración inicial del delay
 * Retorna:
 * 	void
 */
void delayInit(delay_t * delay, tick_t duration)
{
	if(delay == NULL && duration != 0)
	{
		Error_Handler();
	}

	delay->duration = duration;
	delay->running = false;
}

/* delayRead: Lee el tiempo actual y determina si ya transcurrio el retardo esperado
 * Parametros:
 * 	delay_t * delay: puntero a estructura con configuracion del delay
 * Retorna:
 * 	true: si se cumplio el retardo
 * 	false: si todavia no se cumplio el delay
 */
bool_t delayRead(delay_t* delay)
{
	if(delay == NULL)
	{
		Error_Handler();
	}

	if(!delay->running)
	{
		delay->startTime = HAL_GetTick();
		delay->running = true;
	}
	else
	{
		if((HAL_GetTick() - delay->startTime) >= delay->duration)
		{
			delay->running = false;
			return true;
		}
	}
	return false;
}

/* delayWrite: Modifica la duracion de retardo definida al inicializar
 * Parametros:
 * 	delay_t * delay: puntero a estructura con configuración del delay
 * 	tick_t duration: nueva duración del delay
 * Retorna:
 * 	void
 */
void delayWrite(delay_t* delay, tick_t duration)
{
	if(delay == NULL && duration != 0)
	{
		Error_Handler();
	}

	delay->duration = duration;
}

/* delayWrite: Retorna el estado running del delay
 * Parametros:
 * 	delay_t * delay: puntero a estructura con configuración del delay
 * Retorna:
 * 	bool_t: estado de la variable delay->running
 */
bool_t  delayIsRunning(delay_t *delay)
{
	if(delay == NULL)
	{
		Error_Handler();
	}

	return delay->running;
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



