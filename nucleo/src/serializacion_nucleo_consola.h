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

// Mensajes Consola - Nucleo
#define INICIAR 0
#define MATAR 2
#define HANDSHAKE_CONSOLA 3
#define RESPUESTA_IMPRIMIR 13
#define RESPUESTA_IMPRIMIR_TEXTO 14

// Mensajes Nucleo - Consola
#define MENSAJE_HANDSHAKE_RECIBIDO_CONSOLA 13
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4
#define MENSAJE_ERROR_AL_INICIAR 10
#define MENSAJE_MATAR_OK 11
#define MENSAJE_ERROR_AL_MATAR 30
#define MENSAJE_FINALIZO_OK 5


void deserializar_codigo(void* buffer, t_texto *codigo);

#endif /* SERIALIZACION_NUCLE_CONSOLA_H_ */
