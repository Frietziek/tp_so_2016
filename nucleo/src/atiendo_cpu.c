/*
 * atiendo_cpu.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "atiendo_cpu.h"

extern t_queue *cola_block, *cola_exec;

void atender_cpu(t_paquete *paquete, int socket_cpu,
		t_config_nucleo *configuracion) {

	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_HANDSHAKE");
		atiendo_handshake(paquete->payload, socket_cpu);
		printf("Se establecio conexion con cpu\n\n");
		break;
	case MENSAJE_OBTENER_VALOR_COMPARTIDA:
		log_info(logger_manager,
				"Se recibe del cpu: MENSAJE_OBTENER_VALOR_COMPARTIDA");
		atiendo_obtener_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA:
		log_info(logger_manager,
				"Se recibe del cpu: MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA");
		atiendo_asignar_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_IMPRIMIR:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_IMPRIMIR");
		atiendo_imprimir(paquete->payload, socket_cpu);
		break;
	case MENSAJE_IMPRIMIR_TEXTO:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_IMPRIMIR_TEXTO");
		atiendo_imprimir_texto(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ENTRADA_SALIDA:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_ENTRADA_SALIDA");
		atiendo_entrada_salida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_WAIT:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_WAIT");
		atiendo_wait(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_SIGNAL:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_SIGNAL");
		atiendo_signal(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_QUANTUM:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_QUANTUM");
		atiendo_quantum(paquete->payload, socket_cpu);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		log_info(logger_manager,
				"Se recibe del cpu: MENSAJE_PROGRAMA_FINALIZADO");
		atiendo_programa_finalizado(paquete->payload, socket_cpu);
		break;
	case RESPUESTA_MATAR:
		log_info(logger_manager, "Se recibe del cpu: RESPUESTA_MATAR");
		respuesta_matar(paquete->payload, socket_cpu);
		break;
	case MENSAJE_ENTRADA_SALIDA_PCB:
		log_info(logger_manager,
				"Se recibe del cpu: MENSAJE_ENTRADA_SALIDA_PCB");
		atiendo_entrada_salida_pcb(paquete->payload, socket_cpu);
		break;
	case MENSAJE_WAIT_PCB:
		log_info(logger_manager, "Se recibe del cpu: MENSAJE_WAIT_PCB");
		atiendo_wait_pcb(paquete->payload, socket_cpu); //TODO falta hacer
		break;
	case RESPUESTA_PCB:
		log_info(logger_manager, "Se recibe del cpu: RESPUESTA_PCB");
		//bloquear_pcb_semaforo(int socket_cpu);
	}
}

void atiendo_handshake(void *buffer, int socket_conexion) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_NUCLEO;
	header->id_proceso_receptor = PROCESO_CPU;
	header->id_mensaje = REPUESTA_HANDSHAKE;
	header->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, header) < sizeof(header)) {
		perror("Fallo al enviar confirmacion Handshake\n");
	}

	agregar_cpu_disponible(socket_conexion);

	free(header);
}

void atiendo_obtener_compartida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable *variable = malloc(sizeof(t_variable));
	deserializar_variable(buffer, variable);

	t_variable_completa *variable_completa = malloc(
			sizeof(t_variable_completa));
	variable_completa->nombre = variable->nombre;
	variable_completa->valor = obtener_variable_compartida(variable->nombre);

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_OBTENER_VALOR_COMPARTIDA;

	t_buffer *p_cpu = serializar_variable_completa(variable_completa);
	h_cpu->longitud_mensaje = p_cpu->longitud_buffer;
	if (enviar_buffer(socket_conexion, h_cpu, p_cpu)
			< sizeof(h_cpu) + p_cpu->longitud_buffer) {
		log_info(logger_manager,
				"Fallo al enviar Variable Compartida al CPU\n");
	}

	free(variable->nombre);
	free(variable);
	free(variable_completa);
	free(h_cpu);
	free(p_cpu->contenido_buffer);
	free(p_cpu);

}

void atiendo_asignar_compartida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable_completa *variable = malloc(sizeof(t_variable_completa));
	deserializar_variable_completa(buffer, variable);

	asignar_variable_compartida(variable->nombre, variable->valor);

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_ASIGNAR_VARIABLE_COMPARTIDA;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta CPU\n");
	}

	free(variable);
	free(h_cpu);

}

void atiendo_imprimir(void *buffer, int socket_conexion) {

	t_variable_valor *variable = malloc(sizeof(t_valor_variable));
	deserializar_variable_valor(buffer, variable);

	t_header *h_consola = malloc(sizeof(t_header));
	h_consola->id_proceso_emisor = PROCESO_NUCLEO;
	h_consola->id_proceso_receptor = PROCESO_CONSOLA;
	h_consola->id_mensaje = MENSAJE_IMPRIMIR;

	t_buffer *p_consola = serializar_variable_valor(variable);
	h_consola->longitud_mensaje = p_consola->longitud_buffer;

	int socket_consola = buscar_socket_consola_por_socket_cpu(socket_conexion);

	log_info(logger_manager, "Se imprime: %i a Consola: %i", variable->valor,
			socket_consola);

	if (enviar_buffer(socket_consola, h_consola, p_consola)
			< sizeof(h_consola) + p_consola->longitud_buffer) {
		perror("Fallo al enviar Imprimir a la Consola\n");
	}

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_IMPRIMIR;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta a CPU\n");
	}

	free(variable);
	free(h_consola);
	free(p_consola->contenido_buffer);
	free(p_consola);
	free(h_cpu);

}

void atiendo_imprimir_texto(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_texto *texto = malloc(sizeof(t_texto));
	deserializar_texto(buffer, texto);

	t_header *h_consola = malloc(sizeof(t_header));
	h_consola->id_proceso_emisor = PROCESO_NUCLEO;
	h_consola->id_proceso_receptor = PROCESO_CONSOLA;
	h_consola->id_mensaje = MENSAJE_IMPRIMIR_TEXTO;

	t_buffer *p_consola = serializar_texto(texto);
	h_consola->longitud_mensaje = p_consola->longitud_buffer;

	if (enviar_buffer(buscar_socket_consola_por_socket_cpu(socket_conexion),
			h_consola, p_consola)
			< sizeof(h_consola) + p_consola->longitud_buffer) {
		perror("Fallo al enviar Imprimir a la Consola\n");
	}

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_IMPRIMIR_TEXTO;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta a CPU\n");
	}

	free(texto);
	free(h_consola);
	free(p_consola->contenido_buffer);
	free(p_consola);
	free(h_cpu);

}

void atiendo_entrada_salida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_entrada_salida *entrada_salida = malloc(sizeof(t_entrada_salida));
	deserializar_entrada_salida(buffer, entrada_salida);

	sem_wait(&cant_block);

	bloquear_pcb_dispositivo(socket_conexion, entrada_salida);
//asignar_pcb_a_cpu(socket_conexion);

//free(entrada_salida);

}

void atiendo_entrada_salida_pcb(void *buffer, int socket_conexion) {

	t_pcb_quantum *pcb_quantum = malloc(sizeof(t_pcb_quantum));
	deserializar_pcb_quantum(buffer, pcb_quantum);

	sem_wait(&mutex_cola_exec);
	t_pcb * pcb_out = queue_pop_pid(cola_exec, pcb_quantum->pcb->pid);
	sem_post(&mutex_cola_exec);

	sem_wait(&mutex_cola_block);
	queue_push(cola_block, pcb_quantum->pcb);
	log_info(logger_manager, "PROCESO %d - Se agrega a la cola BLOCK",
			pcb_quantum->pcb->pid);
	agregar_cpu_disponible(socket_conexion);
	actualizar_estado_pcb_y_saco_socket_cpu(pcb_quantum->pcb, BLOCK);

	libero_pcb(pcb_out);

	sem_post(&mutex_cola_block);

	sem_post(&cant_block);
}

void atiendo_wait(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
	t_header *h_semaforo;
	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	// MAYOR a 0, sigue con rafaga
	int valor_semaforo = obtener_valor_semaforo(semaforo->nombre);
	if ( valor_semaforo > 0) {
		h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_SEGUI_RAFAGA;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

		decrementar_semaforo(semaforo->nombre);

		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Seguir rafaga al CPU\n");
		}
		free(semaforo->nombre);
		free(semaforo);
	} else {
		sem_wait(&mutex_solicitudes_auxiliares_lista);
		list_add(solicitudes_auxiliares_lista, semaforo);
		sem_post(&mutex_solicitudes_auxiliares_lista);
		//bloquear_pcb_semaforo(semaforo->nombre, socket_conexion);
		h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_WAIT;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;


		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Wait al CPU\n");
		}

	}
	free(h_semaforo);
}

void atiendo_wait_pcb(void *buffer, int socket_conexion) {

	t_pcb_quantum *pcb_quantum = malloc(sizeof(pcb_quantum));
	deserializar_pcb_quantum(buffer, pcb_quantum);

	sem_wait(&mutex_cola_exec);
	queue_pop_pid(cola_exec, pcb_quantum->pcb->pid);

	sem_post(&mutex_cola_exec);

	sem_wait(&mutex_cola_block);
	queue_push(cola_block, pcb_quantum->pcb);
	log_info(logger_manager, "PROCESO %d - Se agrega a la cola BLOCK",
			pcb_quantum->pcb->pid);
	sem_post(&mutex_cola_block);
	actualizar_estado_pcb_y_saco_socket_cpu(pcb_quantum->pcb, BLOCK);
	agregar_cpu_disponible(socket_conexion);

	t_semaforo *semaforo = agregar_solicitud_semaforo_cola_sem(  // para obtener cual era el semaforo a hacer wait por el pcb
			pcb_quantum->pcb->pid);

	t_atributos_semaforo *atributos_semaforo = dictionary_get(
			solitudes_semaforo, semaforo->nombre);

	t_pid *pid = malloc(sizeof(t_pid));
	pid->pid = semaforo->pid;
	queue_push(atributos_semaforo->solicitudes, pid);

	free(semaforo->nombre);
	free(semaforo);
}

t_semaforo *agregar_solicitud_semaforo_cola_sem(int pid) {

	sem_wait(&mutex_solicitudes_auxiliares_lista);

	bool busqueda_semaforo_logica(t_semaforo *semaforo) {
		return (pid == semaforo->pid);
	}
	t_semaforo *semaforo_encontrado = (t_semaforo*) list_remove_by_condition(
			solicitudes_auxiliares_lista, (void*) busqueda_semaforo_logica);

	sem_post(&mutex_solicitudes_auxiliares_lista);
	return semaforo_encontrado;

}

void atiendo_signal(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	aumentar_semaforo(semaforo->nombre);
	int valor_semaforo = obtener_valor_semaforo(semaforo->nombre);

	/*if (valor_semaforo <= 0) {
		//TODO no tengo que hacer nada. creo ...
	} else {
		avisar_para_que_desbloquee(semaforo->nombre);
	}*/
	if (valor_semaforo > 0){
		avisar_para_que_desbloquee(semaforo->nombre);
	}

	t_header *h_semaforo = malloc(sizeof(t_header));
	h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
	h_semaforo->id_proceso_receptor = PROCESO_CPU;
	h_semaforo->id_mensaje = RESPUESTA_SIGNAL;
	h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
		perror("Fallo al responder Signal al CPU\n");
	}

	free(semaforo->nombre);
	free(semaforo);

}

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

void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer,
			&(entrada_salida->nombre_dispositivo), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->tiempo),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->pid),
			&posicion_buffer);

}

void deserializar_semaforo(void *buffer, t_semaforo *semaforo) {
	int posicion_buffer = 0;
	escribir_atributo_desde_string_de_buffer(buffer, &(semaforo->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(semaforo->pid),
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
			+ pcb_quantum->pcb->stack_size * sizeof(t_indice_stack);

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

		copiar_int_en_buffer(buffer, indice_stack->cantidad_argumentos,
				&posicion_buffer);

		if (indice_stack->cantidad_argumentos > 0) {
			cantidad_a_reservar += sizeof(t_posicion_memoria)
					* indice_stack->cantidad_argumentos;
			buffer = (void*) realloc(buffer, cantidad_a_reservar);
		}

		int i_argumentos;
		for (i_argumentos = 0; i_argumentos < indice_stack->cantidad_argumentos;
				++i_argumentos) {

			t_posicion_memoria *indice_argumento = indice_stack->argumentos;
			indice_argumento += i_argumentos;

			copiar_int_en_buffer(buffer, indice_argumento->offset,
					&posicion_buffer);
			copiar_int_en_buffer(buffer, indice_argumento->pagina,
					&posicion_buffer);
			copiar_int_en_buffer(buffer, indice_argumento->size,
					&posicion_buffer);

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

		escribir_atributo_desde_int_de_buffer(buffer,
				&(indice_stack->cantidad_argumentos), &posicion_buffer);

		if (indice_stack->cantidad_argumentos > 0) {
			indice_stack->argumentos = malloc(
					sizeof(t_posicion_memoria)
							* indice_stack->cantidad_argumentos);
		}

		int i_argumentos;
		for (i_argumentos = 0; i_argumentos < indice_stack->cantidad_argumentos;
				++i_argumentos) {

			t_posicion_memoria *indice_argumentos = indice_stack->argumentos;
			indice_argumentos += i_argumentos;

			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_argumentos->offset), &posicion_buffer);
			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_argumentos->pagina), &posicion_buffer);
			escribir_atributo_desde_int_de_buffer(buffer,
					&(indice_argumentos->size), &posicion_buffer);

		}
	}

}

void decrementar_semaforo(char *semaforo_nombre){
	sem_wait(&mutex_solicitudes_semaforo);

	t_atributos_semaforo * attr_sem = (t_atributos_semaforo*) dictionary_get(
			solitudes_semaforo, semaforo_nombre);
	attr_sem->valor--;
	int valor_semaforo = obtener_valor_semaforo(semaforo_nombre);
	sem_post(&mutex_solicitudes_semaforo);
	log_info(logger_manager,"Nuevo valor del semaforo %s= %d",semaforo_nombre,valor_semaforo);
}

void aumentar_semaforo(char *semaforo_nombre){
	sem_wait(&mutex_solicitudes_semaforo);

	t_atributos_semaforo * attr_sem = (t_atributos_semaforo*) dictionary_get(
			solitudes_semaforo, semaforo_nombre);
	attr_sem->valor++;
	int valor_semaforo = obtener_valor_semaforo(semaforo_nombre);

	sem_post(&mutex_solicitudes_semaforo);
	log_info(logger_manager,"Nuevo valor del semaforo %s= %d",semaforo_nombre,valor_semaforo);
}

