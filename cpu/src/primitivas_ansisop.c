/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "primitivas_ansisop.h"

static const int POSICION_MEMORIA = 0x10;

t_puntero ansisop_definir_variable(t_nombre_variable variable) {
	log_info(logger_manager, "Se define la variable %c.", variable);

	t_pcb *pcb = pcb_quantum->pcb;

	if (pcb_quantum->pcb->stack_size == VACIO) {
		pcb_quantum->pcb->indice_stack = malloc(sizeof(t_indice_stack));
		contexto_actual = 0;
		pcb_quantum->pcb->indice_stack->cantidad_argumentos = 0;
		pcb_quantum->pcb->indice_stack->cantidad_variables = 0;
		pcb_quantum->pcb->indice_stack->argumentos = malloc(
				sizeof(t_posicion_memoria));
		pcb_quantum->pcb->indice_stack->posicion_variable_retorno = malloc(
				sizeof(t_posicion_memoria));
		pcb_quantum->pcb->indice_stack->posicion_retorno = 0;
		++pcb_quantum->pcb->stack_size;
	}
	int cantidad_variables = pcb_quantum->pcb->indice_stack->cantidad_variables;
	if (cantidad_variables == 0) {
		pcb_quantum->pcb->indice_stack->variables = malloc(
				sizeof(t_variables_stack));
	} else {
		pcb_quantum->pcb->indice_stack->variables =
				(t_variables_stack*) realloc(
						pcb_quantum->pcb->indice_stack->variables,
						sizeof(t_variables_stack) * (cantidad_variables + 1));
	}
	t_variables_stack *indice_variables =
			pcb_quantum->pcb->indice_stack->variables;
	indice_variables += cantidad_variables;
	indice_variables->id = variable;
	indice_variables->posicion_memoria = malloc(sizeof(t_posicion_memoria));
	indice_variables->posicion_memoria->pagina = calcula_pagina(
			pcb_quantum->pcb->stack_pointer);
	indice_variables->posicion_memoria->offset = calcula_offset(
			pcb_quantum->pcb->stack_pointer);
	indice_variables->posicion_memoria->size = sizeof(int);

	++pcb_quantum->pcb->indice_stack->cantidad_variables;

	// TODO Ver como funciona esto

	log_info(logger_manager, "En la posicion %i.",
			pcb_quantum->pcb->stack_pointer);

	sem_post(&s_instruccion_finalizada);

	pcb_quantum->pcb->stack_pointer += sizeof(int);

	return pcb_quantum->pcb->stack_pointer;
}

t_puntero ansisop_obtener_posicion_variable(t_nombre_variable variable) {
	log_info(logger_manager, "La posicion de la variable %c.", variable);

	int posicion_memoria = -1;
	int pos_variable;
	t_variables_stack *puntero_variable =
			pcb_quantum->pcb->indice_stack->variables;
	for (pos_variable = 0;
			pos_variable < pcb_quantum->pcb->indice_stack->cantidad_variables;
			++pos_variable) {
		puntero_variable += pos_variable;
		if (puntero_variable->id == variable) {
			posicion_memoria = puntero_variable->posicion_memoria->pagina
					* tamanio_pagina
					+ puntero_variable->posicion_memoria->offset;
		}
	}
	log_info(logger_manager, "Es %i.", posicion_memoria);
	sem_post(&s_instruccion_finalizada);

	return posicion_memoria;
}

t_valor_variable ansisop_derefenciar(t_puntero direccion_variable) {
	int contenido_variable = 0;
	log_info(logger_manager, "Dereferencia de: %d ", direccion_variable);

	t_pagina *p_pagina = malloc(sizeof(t_pagina));
	p_pagina->pagina = calcula_pagina(direccion_variable);
	p_pagina->offset = calcula_offset(direccion_variable);
	p_pagina->tamanio = sizeof(int);
	p_pagina->socket_pedido = socket_umc;
	t_buffer *buffer = serializar_pagina(p_pagina);

	pagina_es_codigo = 0;

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC, MENSAJE_LEER_PAGINA,
			"Fallo al enviar lectura de pagina a UMC.", buffer);

	free(p_pagina);
	free(buffer);

	// TODO Descomentar para probar con procesos
	//sem_wait(&s_variable_stack);

	memcpy(contenido_variable, valor_pagina, size_pagina);
	log_info(logger_manager, "Su valor es: %i.", contenido_variable);

	sem_post(&s_instruccion_finalizada);

	return contenido_variable;
}

void ansisop_asignar(t_puntero direccion, t_valor_variable valor) {
	log_info(logger_manager, "Asignando en: %d el valor: %i.", direccion,
			valor);

	t_pagina_completa *p_pagina = malloc(sizeof(t_pagina_completa));
	p_pagina->pagina = calcula_pagina(direccion);
	p_pagina->offset = calcula_offset(direccion);
	p_pagina->tamanio = sizeof(int);
	p_pagina->valor = malloc(sizeof(int));
	memcpy(p_pagina->valor, &valor, p_pagina->tamanio);
	p_pagina->socket_pedido = socket_umc;
	t_buffer *buffer = serializar_pagina_completa(p_pagina);

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC,
	MENSAJE_ESCRIBIR_PAGINA, "Fallo al enviar escritura de pagina a UMC.",
			buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_pagina);
	free(buffer);
}

t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable) {
	int contenido_variable;
	log_info(logger_manager, "El nombre de variable compartida es %s ",
			variable);

	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;
	t_buffer *buffer = serializar_variable(p_compartida);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_LEER_COMPARTIDA, "Fallo al enviar lectura de compartida a Nucleo.",
			buffer);

	free(p_compartida);
	free(buffer);

	sem_wait(&s_variable_compartida);

	memcpy(contenido_variable, valor_pagina, size_pagina);
	log_info(logger_manager, "Su valor es: %i.", contenido_variable);

	sem_post(&s_instruccion_finalizada);

	return contenido_variable;
}

t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	log_info(logger_manager, "Asigno en variable compartida: %s el valor: %d.",
			variable, valor);

	t_variable_completa *p_compartida = malloc(sizeof(t_variable_completa));
	p_compartida->nombre = variable;
	p_compartida->valor = valor;
	t_buffer *buffer = serializar_variable_completa(p_compartida);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_ESCRIBIR_COMPARTIDA,
			"Fallo al enviar escritura de compartida a Nucleo", buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_compartida);
	free(buffer);

	return valor;
}

void ansisop_ir_a_label(t_nombre_etiqueta etiqueta) {
	// TODO Terminar funcion
	log_info(logger_manager, "Voy a la etiqueta: %s.", etiqueta);

	sem_post(&s_instruccion_finalizada);

}

t_puntero_instruccion ansisop_llamar_funcion(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion) {
	// TODO Terminar funcion
	log_info(logger_manager,
			"Llamo a funcion de etiqueta: %s en retorno: %c y posicion de instruccion %c.",
			etiqueta, donde_retornar, linea_en_ejecucion);

	sem_post(&s_instruccion_finalizada);

	return POSICION_MEMORIA;
}

void ansisop_retornar(t_valor_variable retorno) {
	// TODO Terminar funcion
	log_info(logger_manager, "Retorno el valor de la variable %d.", retorno);

	sem_post(&s_instruccion_finalizada);
}

void ansisop_llamar_con_retorno(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar) {
	// TODO Terminar funcion
	log_info(logger_manager,
			"Llamar con retorno en etiqueta: %s y puntero: %d.", etiqueta,
			donde_retornar);

	int puntero;
	puntero = metadata_buscar_etiqueta(etiqueta, pcb_quantum->pcb->etiquetas, pcb_quantum->pcb->etiquetas_size);

	log_info(logger_manager, "Puntero de etiqueta: %d", puntero);

	sem_post(&s_instruccion_finalizada);
}

void ansisop_finalizar() {
	log_info(logger_manager, "Fin de la funcion.");

	sem_post(&s_instruccion_finalizada);
}

void ansisop_imprimir(t_valor_variable valor_mostrar) {
	log_info(logger_manager, "Imprimo el valor: %d.", valor_mostrar);

	t_variable_valor *p_variable = malloc(sizeof(t_variable_valor));
	p_variable->valor = valor_mostrar;
	t_buffer *buffer = serializar_variable_valor(p_variable);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_IMPRIMIR, "Fallo al enviar imprimir a Nucleo.", buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_variable);
	free(buffer);
}

void ansisop_imprimir_texto(char* texto) {
	log_info(logger_manager, "Imprimo el texto: %s.", texto);

	t_texto *p_texto = malloc(sizeof(t_texto));
	p_texto->texto = texto;
	t_buffer *buffer = serializar_texto(p_texto);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_IMPRIMIR_TEXTO, "Fallo al enviar imprimir texto a Nucleo.", buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_texto);
	free(buffer);
}

void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_info(logger_manager, "I/O con dispositivo: %s  y tiempo: %d.",
			dispositivo, tiempo);

	t_entrada_salida *p_entrada_salida = malloc(sizeof(t_entrada_salida));
	p_entrada_salida->nombre_dispositivo = dispositivo;
	p_entrada_salida->tiempo = tiempo;
	t_buffer *buffer = serializar_entrada_salida(p_entrada_salida);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_ENTRADA_SALIDA, "Fallo al enviar I/O a Nucleo.", buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_entrada_salida);
	free(buffer);
}

void ansisop_wait(t_nombre_semaforo semaforo) {
	log_info(logger_manager, "Espero al semaforo: %s.", semaforo);

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;
	t_buffer *buffer = serializar_semaforo(p_semaforo);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO, MENSAJE_WAIT,
			"Fallo al enviar wait al Nucleo.", buffer);

	wait_nucleo = 1;

	sem_post(&s_instruccion_finalizada);

	free(p_semaforo);
	free(buffer);
}

void ansisop_signal(t_nombre_semaforo semaforo) {
	log_info(logger_manager, "Activo al Semaforo: %s.", semaforo);

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;
	t_buffer *buffer = serializar_semaforo(p_semaforo);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_SIGNAL, "Fallo al enviar signal al Nucleo.", buffer);

	sem_post(&s_instruccion_finalizada);

	free(p_semaforo);
	free(buffer);
}
