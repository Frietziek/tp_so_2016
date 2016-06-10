/*
 * serializacion_nucleo_umc.c
 *
 *  Created on: 28/5/2016
 *      Author: utnso
 */
#include <serializacion.h>
#include "serializacion_nucleo_umc.h"

t_buffer *serializar_programa_completo(t_programa_completo *programa) {

	int cantidad_a_reservar = sizeof(programa->id_programa)
			+ sizeof(programa->paginas_requeridas);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, programa->id_programa, &posicion_buffer);
	copiar_int_en_buffer(buffer, programa->paginas_requeridas,
			&posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_programa_completo(void *buffer, t_programa_completo *programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(programa->paginas_requeridas), &posicion_buffer);

}

void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio),
			&posicion_buffer);
}

t_buffer *serializar_pagina_tamanio(t_pagina_tamanio *pagina_tamanio) {

	int cantidad_a_reservar = sizeof(pagina_tamanio->tamanio);

	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, pagina_tamanio->tamanio, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

t_buffer *serializar_finalizar(t_finalizar *finalizar) {

	int cantidad_a_reservar = sizeof(finalizar->pid);

	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, finalizar->pid, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_finalizar(void *buffer, t_finalizar *finalizar) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(finalizar->pid),
			&posicion_buffer);
}

