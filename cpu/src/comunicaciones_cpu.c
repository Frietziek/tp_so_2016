/*
 * comunicaciones_cpu.c
 *
 *  Created on: 14/6/2016
 *      Author: utnso
 */

#include "comunicaciones_cpu.h"

// Envio buffer y header a Procesos
void envio_header_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_proceso, header) < sizeof(t_header)) {
		log_error(logger_manager, mensaje_fallo_envio);
	}

	free(header);
}

void envio_buffer_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio, t_buffer *buffer) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = buffer->longitud_buffer;

	if (enviar_buffer(socket_proceso, header, buffer)
			< sizeof(t_header) + buffer->longitud_buffer) {
		log_error(logger_manager, mensaje_fallo_envio);
	}

	free(header);
}
