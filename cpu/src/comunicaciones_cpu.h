/*
 * comunicaciones_cpu.h
 *
 *  Created on: 14/6/2016
 *      Author: utnso
 */

#ifndef COMUNICACIONES_CPU_H_
#define COMUNICACIONES_CPU_H_

#include <serializacion.h>
#include "semaforo_sockets_cpu.h"

// Envio buffer y header a Procesos
void envio_header_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio);
void envio_buffer_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio, t_buffer *buffer);

#endif /* COMUNICACIONES_CPU_H_ */
