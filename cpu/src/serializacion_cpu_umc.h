/*
 * serializacion_cpu_umc.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_CPU_H_
#define SERIALIZACION_CPU_H_

typedef struct {
	char *nombre;
} t_variable_simple;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_dereferenciar_variable;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int valor;
} t_variable_en_memoria;

t_buffer serializar_variable(t_variable variable);
void deserializar_variable(void *buffer, t_variable *variable);

t_buffer serializar_derefernciar(t_dereferenciar_variable *derefenciar);
void deserializar_derefenciar(void *buffer, t_dereferenciar_variable *derefenciar);

t_buffer serializar_asignar(t_variable_en_memoria *asignar);
void deserializar_asignar(void *buffer, t_variable_en_memoria *asignar);

#endif /* SERIALIZACION_CPU_H_ */
