/*
 * serializacion_umc_swap.c
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#include <serializacion.h>
#include "serializacion_swap_umc.h"

void deserializar_inicio_programa(void *buffer,
		t_programa_completo *inicio_programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer,
			&(inicio_programa->id_programa), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(inicio_programa->paginas_requeridas), &posicion_buffer);

}

void deserializar_leer_pagina(void *buffer, t_pagina *leer_pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(leer_pagina->tamanio),
			&posicion_buffer);

}

void deserializar_escribir_pagina(void *buffer,
		t_pagina_completa *escribir_pagina) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->pagina),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->offset),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(escribir_pagina->tamanio),
			&posicion_buffer);
	// TODO Hacer la funcion de escribir_atributo_desde_void_en_buffer en la libreria de serializacion
	//escribir_atributo_desde_void_de_buffer(buffer, &(escribir_pagina->buffer),
	//		&posicion_buffer);

}

void deserializar_finalizar_programa(void *buffer, t_programa *finalizar_programa) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer,&(finalizar_programa->id_programa), &posicion_buffer);

}

