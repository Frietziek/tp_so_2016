/*
 * serializacion_umc_swap.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <serializacion.h>
#include "serializacion_umc_swap.h"

t_buffer *serializar_inicio_programa(t_inicio_programa *inicio_programa) {

	int cantidad_a_reservar = sizeof(inicio_programa->id_programa)
			+ sizeof(inicio_programa->paginas_requeridas);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, inicio_programa->id_programa,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, inicio_programa->paginas_requeridas,
			&posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_inicio_programa(void *buffer,
		t_inicio_programa *inicio_programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer,
			&(inicio_programa->id_programa), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(inicio_programa->paginas_requeridas), &posicion_buffer);

}

t_buffer *serializar_leer_pagina(t_leer_pagina *leer_pagina) {

	int cantidad_a_reservar = sizeof(leer_pagina->pagina)
			+ sizeof(leer_pagina->offset) + sizeof(leer_pagina->tamanio);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, leer_pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, leer_pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, leer_pagina->tamanio, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_leer_pagina(void *buffer, t_leer_pagina *leer_pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->tamanio),
			&posicion_buffer);

}

t_buffer *serializar_escribir_pagina(t_escribir_pagina *escribir_pagina) {

	int cantidad_a_reservar = sizeof(escribir_pagina->pagina)
			+ sizeof(escribir_pagina->offset) + sizeof(escribir_pagina->tamanio)
			+ sizeof(escribir_pagina->valor);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, escribir_pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, escribir_pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, escribir_pagina->tamanio, &posicion_buffer);
	copiar_int_en_buffer(buffer, escribir_pagina->valor, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_escribir_pagina(void *buffer,
		t_escribir_pagina *escribir_pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->tamanio),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->valor),
			&posicion_buffer);

}

t_buffer *serializar_finalizar_programa(t_fin_programa *finalizar_programa) {

	int cantidad_a_reservar = sizeof(finalizar_programa->id_programa);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, finalizar_programa->id_programa,
			&posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_finalizar_programa(void *buffer,
		t_fin_programa *finalizar_programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer,
			&(finalizar_programa->id_programa), &posicion_buffer);

}

