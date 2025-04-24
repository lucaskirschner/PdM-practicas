/**
 * @file API_delay.h
 * @author Ing. Lucas Kirschner
 * @date 20 Mar 2025
 * @brief Módulo de manejo de retardos no bloqueantes utilizando el temporizador del sistema (SysTick).
 *
 * Proporciona una estructura de delay y funciones asociadas para inicialización, lectura,
 * escritura y verificación del estado del retardo. Este enfoque permite realizar tareas de
 * temporización sin detener la ejecución del programa.
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @struct delay_t
 * @brief Estructura para manejar retardos no bloqueantes.
 */
typedef struct {
   uint32_t startTime; 	/**< Tiempo de inicio del retardo */
   uint32_t duration;  	/**< Duración del retardo */
   bool running;   		/**< Estado del retardo (activo o no) */
} delay_t;

/**
 * @brief Inicializa la estructura de delay con una duración determinada.
 * @param delay Puntero a la estructura delay_t.
 * @param duration Duración del retardo en milisegundos.
 */
void Delay_Init(delay_t * delay, uint32_t duration);

/**
 * @brief Modifica la duración del retardo.
 * @param delay Puntero a la estructura delay_t.
 * @param duration Nueva duración del retardo en milisegundos.
 */
void Delay_Write(delay_t * delay, uint32_t duration);

/**
 * @brief Verifica si se cumplió el retardo.
 * @param delay Puntero a la estructura delay_t.
 * @return true si se cumplió el retardo, false si aún no se completó.
 */
bool Delay_Read(delay_t * delay);

/**
 * @brief Verifica si el retardo se encuentra en ejecución.
 * @param delay Puntero a la estructura delay_t.
 * @return true si está en ejecución, false si no.
 */
bool Delay_Is_Running(delay_t *delay);

#endif /* API_INC_API_DELAY_H_ */
