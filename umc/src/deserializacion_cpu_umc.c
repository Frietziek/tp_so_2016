/*
 * deserializacion_cpu_umc.c
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */

#include "deserializacion_cpu_umc.h"

/*void deserializar_variable(void *buffer, t_variable_simple *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);

}*/

void deserializar_derefenciar(void *buffer,
		t_pagina *derefenciar) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(derefenciar->tamanio),
			&posicion_buffer);
}

void deserializar_asignar(void *buffer, t_pagina_completa *asignar) {
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
