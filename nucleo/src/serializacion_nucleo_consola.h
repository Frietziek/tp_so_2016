/*
 * serializacion_nucle_consola.h
 *
 *  Created on: 22/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_NUCLEO_CONSOLA_H_
#define SERIALIZACION_NUCLEO_CONSOLA_H_

#include <serializacion.h>
#include "atiendo_cpu.h"

// Funciones Consola - Nucleo
#define CODIGO 0
#define FINALIZAR 2

// Funciones Nucleo - Consola
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4

void deserializar_codigo(void* buffer, t_texto *codigo);

#endif /* SERIALIZACION_NUCLE_CONSOLA_H_ */
