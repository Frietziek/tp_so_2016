/*
 * serializacion_umc_swap.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <serializacion.h>
#include "serializacion_umc_swap.h"

t_buffer *serializar_programa_nuevo(t_programa_nuevo *programa){
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));

	int cantidad_a_reservar = sizeof(programa->id_programa)
			+ sizeof(programa->paginas_requeridas);

	estructura_buffer->longitud_buffer = cantidad_a_reservar;

	estructura_buffer->contenido_buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->id_programa, &posicion_buffer);
	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->paginas_requeridas, &posicion_buffer);
	return (estructura_buffer);
}

void deserializar_programa_nuevo(void *buffer, t_programa_nuevo *programa){
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(programa->paginas_requeridas), &posicion_buffer);
}

t_buffer *serializar_programa_completo(t_programa_completo *programa) {

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));

	int cantidad_a_reservar = sizeof(programa->id_programa)
			+ sizeof(programa->paginas_requeridas) + sizeof(int)
			+ strlen(programa->codigo);

	estructura_buffer->longitud_buffer = cantidad_a_reservar;

	estructura_buffer->contenido_buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->id_programa, &posicion_buffer);
	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->paginas_requeridas, &posicion_buffer);
	copiar_string_en_buffer(estructura_buffer->contenido_buffer,
			programa->codigo, &posicion_buffer);
	return (estructura_buffer);
}

void deserializar_programa_completo(void *buffer, t_programa_completo *programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(programa->paginas_requeridas), &posicion_buffer);
	escribir_atributo_desde_string_de_buffer(buffer, &(programa->codigo),
			&posicion_buffer);

}

t_buffer *serializar_programa_para_escritura(t_programa_para_escritura *programa) {

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));

	int cantidad_a_reservar = sizeof(programa->id_programa)
			+ sizeof(programa->nro_pagina_inicial) + sizeof(int)
			+ strlen(programa->contenido);

	estructura_buffer->longitud_buffer = cantidad_a_reservar;

	estructura_buffer->contenido_buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->id_programa, &posicion_buffer);
	copiar_int_en_buffer(estructura_buffer->contenido_buffer,
			programa->nro_pagina_inicial, &posicion_buffer);
	copiar_string_en_buffer(estructura_buffer->contenido_buffer,
			programa->contenido, &posicion_buffer);
	return (estructura_buffer);
}

void deserializar_programa_para_escritura(void *buffer, t_programa_para_escritura *programa){

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(programa->nro_pagina_inicial), &posicion_buffer);
	escribir_atributo_desde_string_de_buffer(buffer, &(programa->contenido),
			&posicion_buffer);

}

t_buffer *serializar_pagina(t_pagina *pagina) {

	int cantidad_a_reservar = sizeof(pagina->id_programa)
			+ sizeof(pagina->pagina) + sizeof(pagina->offset)
			+ sizeof(pagina->tamanio) + sizeof(pagina->socket_pedido);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, pagina->id_programa, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->tamanio, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->socket_pedido, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_pagina(void *buffer, t_pagina *pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido),
			&posicion_buffer);

}

t_buffer *serializar_pagina_completa(t_pagina_completa *pagina) {

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));

	int cantidad_a_reservar = sizeof(pagina->id_programa)
			+ sizeof(pagina->pagina) + sizeof(pagina->offset)
			+ sizeof(pagina->tamanio)
			+ sizeof(pagina->socket_pedido) + sizeof(int) + strlen(pagina->valor);
	void *buffer = malloc(cantidad_a_reservar);

	estructura_buffer->longitud_buffer = cantidad_a_reservar;

	estructura_buffer->contenido_buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, pagina->id_programa, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->tamanio, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->socket_pedido, &posicion_buffer);
	copiar_string_en_buffer(estructura_buffer->contenido_buffer,
			pagina->valor, &posicion_buffer);

	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->id_programa),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido),
			&posicion_buffer);
	escribir_atributo_desde_string_de_buffer(buffer, &(pagina->valor),
			&posicion_buffer);


}

t_buffer *serializar_programa(t_programa *programa) {

	int cantidad_a_reservar = sizeof(programa->id_programa);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, programa->id_programa, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_programa(void *buffer, t_programa *programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa),
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

void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio),
			&posicion_buffer);
}

t_buffer *serializar_pagina_pedido(t_pagina_pedido *pagina) {
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
void deserializar_pagina_pedido(void *buffer, t_pagina_pedido *pagina) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
}
t_buffer *serializar_pagina_pedido_completa(t_pagina_pedido_completa *pagina) {
	int cantidad_a_reservar = sizeof(pagina->pagina) + sizeof(pagina->offset)
			+ sizeof(pagina->tamanio) + pagina->tamanio;
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, pagina->pagina, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->offset, &posicion_buffer);
	copiar_int_en_buffer(buffer, pagina->tamanio, &posicion_buffer);
	memcpy(buffer + posicion_buffer, pagina->valor, pagina->tamanio);
	posicion_buffer = posicion_buffer + pagina->tamanio;
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_pagina_pedido_completa(void *buffer,
		t_pagina_pedido_completa *pagina) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio),
			&posicion_buffer);
	pagina->valor = malloc(pagina->tamanio);
	memcpy(pagina->valor, buffer + posicion_buffer, pagina->tamanio);
	posicion_buffer = posicion_buffer + pagina->tamanio;
}
