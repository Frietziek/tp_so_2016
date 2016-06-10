/*
 * serializacion_nucleo_consola.c
 *
 *  Created on: 22/5/2016
 *      Author: utnso
 */
#include "serializacion_nucleo_consola.h"

void deserializar_codigo(void* buffer, t_texto *codigo) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(codigo->texto),
			&posicion_buffer);
}
