/*
 * serializaciones_cpu.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "serializaciones_cpu.h"

// Funciones CPU - Nucleo
t_buffer *serializar_variable(t_variable *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_variable(void *buffer, t_variable *variable) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
}
t_buffer *serializar_variable_completa(t_variable_completa *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre)
			+ sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);
	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_variable_completa(void *buffer, t_variable_completa *variable) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);
}
t_buffer *serializar_variable_valor(t_variable_valor *variable) {
	int cantidad_a_reservar = sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_variable_valor(void *buffer, t_variable_valor *variable) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);
}

t_buffer *serializar_texto(t_texto *texto) {
	int cantidad_a_reservar = sizeof(int) + strlen(texto->texto);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, texto->texto, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_texto(void *buffer, t_texto *texto) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(texto->texto),
			&posicion_buffer);
}
t_buffer *serializar_entrada_salida(t_entrada_salida *entrada_salida) {
	int cantidad_a_reservar = sizeof(int)
			+ strlen(entrada_salida->nombre_dispositivo)
			+ sizeof(entrada_salida->tiempo) + sizeof(entrada_salida->pid);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, entrada_salida->nombre_dispositivo,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, entrada_salida->tiempo, &posicion_buffer);
	copiar_int_en_buffer(buffer, entrada_salida->pid, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer,
			&(entrada_salida->nombre_dispositivo), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->tiempo),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->pid),
			&posicion_buffer);
}
t_buffer *serializar_semaforo(t_semaforo *semaforo) {
	int cantidad_a_reservar = sizeof(int) + sizeof(int)
			+ strlen(semaforo->nombre);
	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_string_en_buffer(buffer, semaforo->nombre, &posicion_buffer);
	copiar_int_en_buffer(buffer, semaforo->pid, &posicion_buffer);
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_semaforo(void *buffer, t_semaforo *semaforo) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(semaforo->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(semaforo->pid),
			&posicion_buffer);
}
// Funciones CPU - UMC
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
void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pagina_tamanio->tamanio),
			&posicion_buffer);
}
t_buffer *serializar_pcb_quantum(t_pcb_quantum *pcb_quantum) {
	int cantidad_a_reservar = sizeof(pcb_quantum->quantum)
			+ sizeof(pcb_quantum->pcb->pid) + sizeof(pcb_quantum->pcb->pc)
			+ sizeof(pcb_quantum->pcb->cant_paginas_codigo_stack)
			+ sizeof(pcb_quantum->pcb->estado)
			+ sizeof(pcb_quantum->pcb->contexto_actual)
			+ sizeof(pcb_quantum->pcb->stack_size_fisico)
			+ sizeof(pcb_quantum->pcb->stack_position)
			+ sizeof(pcb_quantum->pcb->stack_pointer)
			+ sizeof(pcb_quantum->pcb->etiquetas_size)
			+ pcb_quantum->pcb->etiquetas_size
			+ sizeof(pcb_quantum->pcb->instrucciones_size)
			+ pcb_quantum->pcb->instrucciones_size * sizeof(t_intructions)
			+ sizeof(pcb_quantum->pcb->stack_size)
			+ pcb_quantum->pcb->stack_size
					* (sizeof(int) + sizeof(int) * 3 + sizeof(int));

	void *buffer = malloc(cantidad_a_reservar);
	int posicion_buffer = 0;
	copiar_int_en_buffer(buffer, pcb_quantum->quantum, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->pid, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->pc, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->cant_paginas_codigo_stack,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->estado, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->contexto_actual,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->stack_size_fisico,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->stack_position,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->stack_pointer,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->etiquetas_size,
			&posicion_buffer);
	memcpy(buffer + posicion_buffer, pcb_quantum->pcb->etiquetas,
			pcb_quantum->pcb->etiquetas_size);
	posicion_buffer += pcb_quantum->pcb->etiquetas_size;
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->instrucciones_size,
			&posicion_buffer);
	int i_instrucciones;
	for (i_instrucciones = 0;
			i_instrucciones < pcb_quantum->pcb->instrucciones_size;
			++i_instrucciones) {
		t_intructions *instrucciones_serializadas =
				pcb_quantum->pcb->instrucciones_serializadas;
		instrucciones_serializadas += i_instrucciones;
		copiar_int_en_buffer(buffer, instrucciones_serializadas->start,
				&posicion_buffer);
		copiar_int_en_buffer(buffer, instrucciones_serializadas->offset,
				&posicion_buffer);
	}
	copiar_int_en_buffer(buffer, pcb_quantum->pcb->stack_size,
			&posicion_buffer);
	int i_stack;
	for (i_stack = 0; i_stack < pcb_quantum->pcb->stack_size; ++i_stack) {
		t_indice_stack *indice_stack = pcb_quantum->pcb->indice_stack;
		indice_stack += i_stack;
		copiar_int_en_buffer(buffer, indice_stack->posicion_retorno,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				indice_stack->posicion_variable_retorno->offset,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				indice_stack->posicion_variable_retorno->pagina,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				indice_stack->posicion_variable_retorno->size,
				&posicion_buffer);
		copiar_int_en_buffer(buffer, indice_stack->cantidad_variables,
				&posicion_buffer);
		if (indice_stack->cantidad_variables > 0) {
			cantidad_a_reservar += (sizeof(char) + sizeof(int) * 3)
					* indice_stack->cantidad_variables;
			buffer = (void*) realloc(buffer, cantidad_a_reservar);
		}
		int i_variables;
		for (i_variables = 0; i_variables < indice_stack->cantidad_variables;
				++i_variables) {
			t_variables_stack *indice_variables = indice_stack->variables;
			indice_variables += i_variables;

			copiar_char_en_buffer(buffer, indice_variables->id,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					indice_variables->posicion_memoria->offset,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					indice_variables->posicion_memoria->pagina,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					indice_variables->posicion_memoria->size, &posicion_buffer);

		}
	}
	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;
	return (estructura_buffer);
}
void deserializar_pcb_quantum(void *buffer, t_pcb_quantum *pcb_quantum) {
	int posicion_buffer = 0;
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb_quantum->quantum),
			&posicion_buffer);
	pcb_quantum->pcb = malloc(sizeof(t_pcb));
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb_quantum->pcb->pid),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb_quantum->pcb->pc),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->cant_paginas_codigo_stack), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb_quantum->pcb->estado),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->contexto_actual), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->stack_size_fisico), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->stack_position), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->stack_pointer), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			(int*) &(pcb_quantum->pcb->etiquetas_size), &posicion_buffer);
	pcb_quantum->pcb->etiquetas = malloc(pcb_quantum->pcb->etiquetas_size);
	memcpy(pcb_quantum->pcb->etiquetas, buffer + posicion_buffer,
			pcb_quantum->pcb->etiquetas_size);
	posicion_buffer += pcb_quantum->pcb->etiquetas_size;
	escribir_atributo_desde_int_de_buffer(buffer,
			(int*) &(pcb_quantum->pcb->instrucciones_size), &posicion_buffer);
	int i_instrucciones;
	pcb_quantum->pcb->instrucciones_serializadas = malloc(
			sizeof(t_intructions) * pcb_quantum->pcb->instrucciones_size);
	for (i_instrucciones = 0;
			i_instrucciones < pcb_quantum->pcb->instrucciones_size;
			++i_instrucciones) {
		t_intructions *instrucciones_serializadas =
				pcb_quantum->pcb->instrucciones_serializadas;
		instrucciones_serializadas += i_instrucciones;
		escribir_atributo_desde_int_de_buffer(buffer,
				(int*) &(instrucciones_serializadas->start), &posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				(int*) &(instrucciones_serializadas->offset), &posicion_buffer);
	}
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb_quantum->pcb->stack_size), &posicion_buffer);
	int i_stack;
	pcb_quantum->pcb->indice_stack = malloc(
			sizeof(t_indice_stack) * pcb_quantum->pcb->stack_size);
	for (i_stack = 0; i_stack < pcb_quantum->pcb->stack_size; ++i_stack) {
		t_indice_stack *indice_stack = pcb_quantum->pcb->indice_stack;
		indice_stack += i_stack;
		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->posicion_retorno), &posicion_buffer);
		indice_stack->posicion_variable_retorno = malloc(
				sizeof(t_posicion_memoria));
		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->posicion_variable_retorno->offset),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->posicion_variable_retorno->pagina),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->posicion_variable_retorno->size),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->cantidad_variables), &posicion_buffer);
		if (indice_stack->cantidad_variables > 0) {
			indice_stack->variables = malloc(
					sizeof(t_variables_stack)
							* indice_stack->cantidad_variables);
		}
		int i_variables;
		for (i_variables = 0; i_variables < indice_stack->cantidad_variables;
				++i_variables) {
			t_variables_stack *indice_variables = indice_stack->variables;
			indice_variables += i_variables;
			escribir_atributo_desde_char_de_buffer(buffer,
					&(indice_variables->id), &posicion_buffer);

			indice_variables->posicion_memoria = malloc(
					sizeof(t_posicion_memoria));
			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_variables->posicion_memoria->offset),
					&posicion_buffer);
			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_variables->posicion_memoria->pagina),
					&posicion_buffer);
			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_variables->posicion_memoria->size),
					&posicion_buffer);

		}
	}
}
