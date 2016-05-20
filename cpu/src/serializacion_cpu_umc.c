/*
 * serializacion_cpu_umc.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <commons/string.h>
#include <serializacion.h>
#include "serializacion_cpu_umc.h"

t_buffer serializar_variable(t_variable_simple *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_variable(void *buffer, t_variable_simple *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);

}

t_buffer serializar_derefernciar(t_dereferenciar_variable *derefenciar) {
	int cantidad_a_reservar = sizeof(derefenciar->pagina)
			+ sizeof(derefenciar->offset) + sizeof(derefenciar->tamanio);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, derefenciar->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, derefenciar->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, derefenciar->tamanio, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_derefenciar(void *buffer,
		t_dereferenciar_variable *derefenciar) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->tamanio),
			&posicion_buffer);
}

t_buffer serializar_asignar(t_variable_en_memoria *asignar) {
	int cantidad_a_reservar = sizeof(asignar->pagina) + sizeof(asignar->offset)
			+ sizeof(asignar->tamanio) + sizeof(asignar->valor);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, asignar->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, asignar->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, asignar->tamanio, &posicion_buffer);
	copiar_int_en_buffer(buffer, asignar->valor, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_asignar(void *buffer, t_variable_en_memoria *asignar) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(asignar->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(asignar->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(asignar->tamanio),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(asignar->valor),
			&posicion_buffer);
}
