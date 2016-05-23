/*
 * serializacion_cpu_nucleo.c
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */

#include <commons/string.h>
#include <serializacion.h>
#include "serializacion_cpu_nucleo.h"

t_buffer *serializar_variable_compartida(t_variable *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_variable_compartida(void *buffer,
		t_variable *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);

}

t_buffer *serializar_asignar_variable_compartida(t_variable_completa *variable) {
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

void deserializar_asignar_variable_compartida(void *buffer,
		t_variable_completa *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);

}

t_buffer *serializar_imprimir_variable(t_variable_completa *variable) {
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

void deserializar_imprimir_variable(void *buffer, t_variable_completa *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);

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

void deserializar_imprimir_texto(void *buffer, t_texto *texto) {
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
