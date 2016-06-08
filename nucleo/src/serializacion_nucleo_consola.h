/*
 * serializacion_nucle_consola.h
 *
 *  Created on: 22/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_NUCLEO_CONSOLA_H_
#define SERIALIZACION_NUCLEO_CONSOLA_H_

#include <serializacion.h>

// Funciones Consola - Nucleo
#define CODIGO 1
#define FINALIZAR 2

// Funciones Nucleo - Consola
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4

typedef struct {
	char *texto;
} t_texto;

void deserializar_codigo(void* buffer, t_texto *codigo);

#endif /* SERIALIZACION_NUCLE_CONSOLA_H_ */
