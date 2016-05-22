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

//void deserializar_variable(void *buffer, t_variable_simple *variable);
void deserializar_derefenciar(void *buffer,
		t_dereferenciar_variable *derefenciar);
void deserializar_asignar(void *buffer, t_variable_en_memoria *asignar);

#endif /* DESERIALIZACION_CPU_UMC_H_ */
