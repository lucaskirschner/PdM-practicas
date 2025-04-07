/**
 * @file API_delay.h
 * @author Ing. Lucas Kirschner
 * @date Mar 20, 2025
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
 * @typedef tick_t
 * @brief Representa un valor de tiempo en milisegundos.
 */
typedef uint32_t tick_t;

/**
 * @typedef bool_t
 * @brief Representación booleana personalizada.
 */
typedef bool bool_t;

/**
 * @struct delay_t
 * @brief Estructura para manejar retardos no bloqueantes.
 */
typedef struct {
   tick_t startTime; /**< Tiempo de inicio del retardo */
   tick_t duration;  /**< Duración del retardo */
   bool_t running;   /**< Estado del retardo (activo o no) */
} delay_t;

/**
 * @brief Inicializa la estructura de delay con una duración determinada.
 * @param delay Puntero a la estructura delay_t.
 * @param duration Duración del retardo en milisegundos.
 */
void delayInit(delay_t * delay, tick_t duration);

/**
 * @brief Modifica la duración del retardo.
 * @param delay Puntero a la estructura delay_t.
 * @param duration Nueva duración del retardo en milisegundos.
 */
void delayWrite(delay_t * delay, tick_t duration);

/**
 * @brief Verifica si se cumplió el retardo.
 * @param delay Puntero a la estructura delay_t.
 * @return true si se cumplió el retardo, false si aún no se completó.
 */
bool_t delayRead(delay_t * delay);

/**
 * @brief Verifica si el retardo se encuentra en ejecución.
 * @param delay Puntero a la estructura delay_t.
 * @return true si está en ejecución, false si no.
 */
bool_t delayIsRunning(delay_t *delay);

#endif /* API_INC_API_DELAY_H_ */
