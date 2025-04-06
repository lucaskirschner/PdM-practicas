/*
 * API_swo.c
 *
 *  Created on: Mar 29, 2025
 *      Author: Lucas Kirschner
 */

#include "API_swo.h"
#include "stm32f4xx.h"

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
        ITM_SendChar(*ptr++);  							// Envía cada carácter por SWO

    return len;
}


