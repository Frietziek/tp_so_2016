/*
 * serializacion_cpu_umc.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <serializacion.h>
#include "serializacion_cpu_umc.h"

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
			+ sizeof(pagina->tamanio) + sizeof(pagina->valor);
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
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->valor),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido),
			&posicion_buffer);

}

void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio),
			&posicion_buffer);
}
