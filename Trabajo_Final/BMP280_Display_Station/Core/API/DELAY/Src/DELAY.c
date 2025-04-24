/**
 * @file API_delay.c
 * @author Ing. Lucas Kirschner
 * @date 20 Mar 2025
 * @brief Implementación del módulo de retardos no bloqueantes utilizando HAL_GetTick().
 *
 * Esta implementación permite trabajar con delays sin detener el flujo del programa,
 * ideal para sistemas embebidos donde se requiere temporización precisa sin bloquear.
 */

#include <DELAY.h>
#include "stm32f4xx_hal.h"

static void Error_Handler(void);

void Delay_Init(delay_t * delay, uint32_t duration)
{
	if(delay == NULL && duration != 0)
	{
		Error_Handler();
	}

	delay->duration = duration;
	delay->running = false;
}

bool Delay_Read(delay_t* delay)
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

void Delay_Write(delay_t* delay, uint32_t duration)
{
	if(delay == NULL && duration != 0)
	{
		Error_Handler();
	}

	delay->duration = duration;
}

bool Delay_Is_Running(delay_t *delay)
{
	if(delay == NULL)
	{
		Error_Handler();
	}

	return delay->running;
}

/**
 * @brief Manejador de errores fatales. Desactiva las interrupciones y entra en un bucle infinito.
 */
static void Error_Handler(void)
{
	__disable_irq();
	while (1) {}
}
