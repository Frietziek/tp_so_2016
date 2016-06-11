/*
 * serializaciones_cpu.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "serializaciones_cpu.h"

// Funciones CPU - Nucleo
t_buffer *serializar_variable(t_variable *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_variable(void *buffer, t_variable *variable) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
}
t_buffer *serializar_variable_completa(t_variable_completa *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre)
			+ sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);
	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_variable_completa(void *buffer, t_variable_completa *variable) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);
}
t_buffer *serializar_variable_valor(t_variable_valor *variable) {
	int cantidad_a_reservar = sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);
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

t_buffer *serializar_texto(t_texto *texto) {
	int cantidad_a_reservar = sizeof(int) + strlen(texto->texto);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, texto->texto, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_texto(void *buffer, t_texto *texto) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(texto->texto),
			&posicion_buffer);
}
t_buffer *serializar_entrada_salida(t_entrada_salida *entrada_salida) {
	int cantidad_a_reservar = sizeof(int)
			+ strlen(entrada_salida->nombre_dispositivo)
			+ sizeof(entrada_salida->tiempo);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, entrada_salida->nombre_dispositivo,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, entrada_salida->tiempo, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer,
			&(entrada_salida->nombre_dispositivo), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->tiempo),
			&posicion_buffer);
}
t_buffer *serializar_semaforo(t_semaforo *semaforo) {
	int cantidad_a_reservar = sizeof(int) + strlen(semaforo->nombre);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, semaforo->nombre, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}

void deserializar_semaforo(void *buffer, t_semaforo *entrada_salida) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(entrada_salida->nombre),
			&posicion_buffer);
}
// Funciones CPU - UMC
t_buffer *serializar_pagina(t_pagina *pagina) {
	int cantidad_a_reservar = sizeof(pagina->pagina) + sizeof(pagina->offset)
			+ sizeof(pagina->tamanio);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->tamanio, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_pagina(void *buffer, t_pagina *pagina) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
}
t_buffer *serializar_pagina_completa(t_pagina_completa *pagina) {
	int cantidad_a_reservar = sizeof(pagina->pagina) + sizeof(pagina->offset)
			+ sizeof(pagina->tamanio) + sizeof(pagina->valor)
			+ sizeof(pagina->socket_pedido);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->tamanio, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->valor, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->socket_pedido, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
	memcpy(pagina->valor, buffer, pagina->tamanio);
	posicion_buffer = posicion_buffer + pagina->tamanio;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido),
			&posicion_buffer);
}
void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio),
			&posicion_buffer);
}
