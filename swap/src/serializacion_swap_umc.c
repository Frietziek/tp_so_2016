/*
 * serializacion_umc_swap.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <serializacion.h>
#include "serializacion_swap_umc.h"


void deserializar_programa(void *buffer, t_programa *programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa), &posicion_buffer);

}

void deserializar_programa_completo(void *buffer, t_programa_completo *programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(programa->id_programa), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,&(programa->paginas_requeridas), &posicion_buffer);
	escribir_atributo_desde_string_de_buffer(buffer,&(programa->codigo), &posicion_buffer);

}



void deserializar_pagina(void *buffer, t_pagina *pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->id_programa), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido), &posicion_buffer);

}


void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->id_programa), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->pagina), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->offset), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->tamanio), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->valor), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina->socket_pedido), &posicion_buffer);

}


void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio), &posicion_buffer);
}

