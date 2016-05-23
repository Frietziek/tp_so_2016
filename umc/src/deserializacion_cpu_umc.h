/*
 * deserializacion_cpu_umc.h
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */

#ifndef DESERIALIZACION_CPU_UMC_H_
#define DESERIALIZACION_CPU_UMC_H_

/*typedef struct {
 char *nombre;
 } t_variable_simple;*/

/*typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_pagina;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int valor;
} t_pagina_completa;*/
#include "serializacion_umc_swap.h"

//void deserializar_variable(void *buffer, t_variable_simple *variable);
void deserializar_derefenciar(void *buffer, t_pagina *derefenciar);
void deserializar_asignar(void *buffer, t_pagina_completa *asignar);

#endif /* DESERIALIZACION_CPU_UMC_H_ */
