/**
 * @file API_swo.h
 * @author Ing. Lucas Kirschner
 * @date 29 Mar 2025
 * @brief Redefinición de la función _write para habilitar impresión por consola usando SWO (Serial Wire Output).
 *        Esta interfaz permite enviar datos al debugger a través de ITM (Instrumentation Trace Macrocell).
 */

#ifndef INC_API_SWO_H_
#define INC_API_SWO_H_

#include <stdio.h>

/**
 * @brief Redefinición de la función _write utilizada por printf para redirigir la salida por SWO.
 *
 * Esta función se redefine para permitir la impresión de texto mediante la interfaz ITM,
 * utilizada en herramientas de depuración compatibles como STM32CubeIDE, ST-LINK o J-Link.
 *
 * @param file Descripto de archivo (no se utiliza).
 * @param ptr Puntero al buffer de datos a enviar.
 * @param len Longitud del buffer.
 * @return Número de bytes enviados.
 */
extern int _write(int file, char *ptr, int len);

#endif /* INC_API_SWO_H_ */
