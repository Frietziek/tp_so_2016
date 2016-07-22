/*
 * serializacion_consola_nucleo.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "serializacion_consola_nucleo.h"

void consola_nucleo(int socket_nucleo, int opcion) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CONSOLA;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = opcion;
	header->longitud_mensaje = 0;

	enviar_header(socket_nucleo, header);
	free(header);
}

t_buffer *serializar_imprimir_texto(t_texto *texto) {
	int cantidad_a_reservar = sizeof(int) + strlen(texto->texto);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, texto->texto, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}


void deserializar_variable_valor(void *buffer, t_variable_valor *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);

}

void deserializar_texto(void *buffer, t_texto *texto) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(texto->texto),
			&posicion_buffer);

}
