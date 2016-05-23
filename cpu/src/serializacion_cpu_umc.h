/*
 * serializacion_cpu_umc.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_CPU_H_
#define SERIALIZACION_CPU_H_

/*typedef struct {
	char *nombre;
} t_variable_simple;*/

typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_pagina;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int valor;
} t_pagina_completa;

//t_buffer *serializar_variable(t_variable_simple *variable);

t_buffer *serializar_dereferenciar(t_pagina *derefenciar);

t_buffer *serializar_asignar(t_pagina_completa *asignar);

#endif /* SERIALIZACION_CPU_H_ */
