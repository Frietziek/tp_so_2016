/*
 * atiendo_cpu.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "atiendo_cpu.h"

void atender_cpu(t_paquete *paquete, int socket_cpu,
		t_config_nucleo *configuracion) {

	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		atiendo_handshake(paquete->payload, socket_cpu);
		printf("Se establecio conexion con cpu\n\n");
		break;
	case MENSAJE_OBTENER_VALOR_COMPARTIDA:
		atiendo_obtener_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA:
		atiendo_asignar_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_IMPRIMIR:
		atiendo_imprimir(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_IMPRIMIR_TEXTO:
		atiendo_imprimir_texto(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ENTRADA_SALIDA:
		atiendo_entrada_salida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_WAIT:
		atiendo_wait(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_SIGNAL:
		atiendo_signal(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_QUANTUM:
		atiendo_quantum(paquete->payload, socket_cpu);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		atiendo_programa_finalizado(paquete->payload, socket_cpu);
		break;
	}

	free(paquete);

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
		perror("Fallo al enviar Variable Compartida al CPU\n");
	}

	free(variable);
	free(variable_completa);
	free(h_cpu);
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

void atiendo_imprimir(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable_valor *variable = malloc(sizeof(t_valor_variable));
	deserializar_variable_valor(buffer, variable);

	t_header *h_consola = malloc(sizeof(t_header));
	h_consola->id_proceso_emisor = PROCESO_NUCLEO;
	h_consola->id_proceso_receptor = PROCESO_CONSOLA;
	h_consola->id_mensaje = MENSAJE_IMPRIMIR;

	t_buffer *p_consola = serializar_variable_valor(variable);
	h_consola->longitud_mensaje = p_consola->longitud_buffer;

	if (enviar_buffer(devuelve_socket_consola(socket_conexion), h_consola,
			p_consola) < sizeof(h_consola) + p_consola->longitud_buffer) {
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

	if (enviar_buffer(devuelve_socket_consola(socket_conexion), h_consola,
			p_consola) < sizeof(h_consola) + p_consola->longitud_buffer) {
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
	free(p_consola);
	free(h_cpu);

}

void atiendo_entrada_salida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_entrada_salida *entrada_salida = malloc(sizeof(t_entrada_salida));
	deserializar_entrada_salida(buffer, entrada_salida);

	bloquear_pcb_dispositivo(socket_conexion,
			entrada_salida->nombre_dispositivo, entrada_salida->tiempo);
	asignar_pcb(socket_conexion);//TODO PREGUNTAR SI ES NECESARIO EJECUTARLO AHI

	free(entrada_salida);

}

void atiendo_wait(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	// Distinto de 0, sigue con rafaga
	if (wait_semaforo(semaforo->nombre)) {
		t_header *h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_SEGUI_RAFAGA;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Seguir rafaga al CPU\n");
		}

		free(h_semaforo);
	} else {
		bloquear_pcb_semaforo(semaforo->nombre, socket_conexion);

		t_header *h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_WAIT;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Wait al CPU\n");
		}

		free(h_semaforo);
	}

	free(semaforo);
}

void atiendo_signal(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	signal_semaforo(semaforo->nombre);

	t_header *h_semaforo = malloc(sizeof(t_header));
	h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
	h_semaforo->id_proceso_receptor = PROCESO_CPU;
	h_semaforo->id_mensaje = RESPUESTA_SIGNAL;
	h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
		perror("Fallo al responder Signal al CPU\n");
	}

	free(semaforo);
}

void atiendo_quantum(void *buffer, int socket_conexion) {
	// Recibo PCB

	asignar_pcb(socket_conexion);
}

void atiendo_programa_finalizado(void *buffer, int socket_conexion) {
	// Recibo PCB

	asignar_pcb(socket_conexion);
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

}

void deserializar_semaforo(void *buffer, t_semaforo *entrada_salida) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(entrada_salida->nombre),
			&posicion_buffer);

}//Serializacion pcb

//typedef struct {
//	int pid;
//	int pc;
//	int cant_paginas_codigo_stack;
//	int estado;
//	int stack_size_maximo;
//	int stack_position;
//	t_size etiquetas_size; // Tamaño del mapa serializado de etiquetas
//	char* etiquetas;
//	t_size instrucciones_size;
//	t_intructions **instrucciones_serializadas;
//	int stack_size_actual;
//	t_indice_stack **indice_stack;
//} t_pcb;

t_buffer *serializar_pcb(t_pcb *pcb) {
	int cantidad_a_reservar = sizeof(pcb->pid) + sizeof(pcb->pc)
			+ sizeof(pcb->cant_paginas_codigo_stack) + sizeof(pcb->estado)
			+ sizeof(pcb->stack_size_maximo) + sizeof(pcb->stack_position)
			+ sizeof(int) + strlen(pcb->etiquetas)
			+ sizeof(pcb->instrucciones_size)
			+ sizeof(pcb->instrucciones_serializadas)
			+ sizeof(pcb->stack_size_actual) + sizeof(pcb->indice_stack)
			+ sizeof(int) + sizeof(int); // Cantidad variables + Cantidad argumentos
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, pcb->pid, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb->pc, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb->cant_paginas_codigo_stack,
			&posicion_buffer);
	copiar_int_en_buffer(buffer, pcb->estado, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb->stack_size_maximo, &posicion_buffer);
	copiar_int_en_buffer(buffer, pcb->stack_position, &posicion_buffer);
	copiar_string_en_buffer(buffer, pcb->etiquetas, &posicion_buffer);

	copiar_int_en_buffer(buffer, pcb->instrucciones_size, &posicion_buffer);
	int i_instrucciones;
	for (i_instrucciones = 0; i_instrucciones < pcb->instrucciones_size;
			++i_instrucciones) {
		copiar_int_en_buffer(buffer,
				pcb->instrucciones_serializadas[i_instrucciones]->offset,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				pcb->instrucciones_serializadas[i_instrucciones]->start,
				&posicion_buffer);
	}

	copiar_int_en_buffer(buffer, pcb->stack_size_actual, &posicion_buffer);
	int i_stack;
	for (i_stack = 0; i_stack < pcb->stack_size_actual; ++i_stack) {
		copiar_int_en_buffer(buffer,
				pcb->indice_stack[i_stack]->posicion_retorno, &posicion_buffer);
		copiar_int_en_buffer(buffer,
				pcb->indice_stack[i_stack]->posicion_variable_retorno->offset,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				pcb->indice_stack[i_stack]->posicion_variable_retorno->pagina,
				&posicion_buffer);
		copiar_int_en_buffer(buffer,
				pcb->indice_stack[i_stack]->posicion_variable_retorno->size,
				&posicion_buffer);

		int cant_argumentos =
				pcb->indice_stack[i_stack]->argumentos->elements_count;

		copiar_int_en_buffer(buffer, cant_argumentos, &posicion_buffer);
		int i_argumentos;
		for (i_argumentos = 0; i_argumentos < cant_argumentos; ++i_argumentos) {
			copiar_int_en_buffer(buffer,
					((t_posicion_memoria*) (list_get(
							pcb->indice_stack[i_stack]->argumentos,
							i_argumentos)))->offset, &posicion_buffer);
			copiar_int_en_buffer(buffer,
					((t_posicion_memoria*) (list_get(
							pcb->indice_stack[i_stack]->argumentos,
							i_argumentos)))->pagina, &posicion_buffer);
			copiar_int_en_buffer(buffer,
					((t_posicion_memoria*) (list_get(
							pcb->indice_stack[i_stack]->argumentos,
							i_argumentos)))->size, &posicion_buffer);
		}

		int cant_variables =
				pcb->indice_stack[i_stack]->variables->elements_count;
		copiar_int_en_buffer(buffer, cant_variables, &posicion_buffer);
		int i_variables;
		for (i_variables = 0; i_variables < cant_variables; ++i_variables) {
			copiar_int_en_buffer(buffer,
					((t_variables_stack*) (list_get(
							pcb->indice_stack[i_stack]->variables, i_variables)))->id,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					((t_variables_stack*) (list_get(
							pcb->indice_stack[i_stack]->variables, i_variables)))->posicion_memoria->offset,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					((t_variables_stack*) (list_get(
							pcb->indice_stack[i_stack]->variables, i_variables)))->posicion_memoria->pagina,
					&posicion_buffer);
			copiar_int_en_buffer(buffer,
					((t_variables_stack*) (list_get(
							pcb->indice_stack[i_stack]->variables, i_variables)))->posicion_memoria->size,
					&posicion_buffer);

		}

	}

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_pcb(void *buffer, t_pcb *pcb) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->pid),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->pc), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer,
			&(pcb->cant_paginas_codigo_stack), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->estado),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->stack_size_maximo),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->stack_position),
			&posicion_buffer);
	escribir_atributo_desde_string_de_buffer(buffer, &(pcb->etiquetas),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->instrucciones_size),
			&posicion_buffer);
	int i_instrucciones;
	for (i_instrucciones = 0; i_instrucciones < pcb->instrucciones_size;
			++i_instrucciones) {
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->instrucciones_serializadas[i_instrucciones]->offset),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->instrucciones_serializadas[i_instrucciones]->start),
				&posicion_buffer);
	}

	escribir_atributo_desde_int_de_buffer(buffer, &(pcb->stack_size_actual),
			&posicion_buffer);
	int i_stack;
	for (i_stack = 0; i_stack < pcb->stack_size_actual; ++i_stack) {
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->indice_stack[i_stack]->posicion_retorno),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->indice_stack[i_stack]->posicion_variable_retorno->offset),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->indice_stack[i_stack]->posicion_variable_retorno->pagina),
				&posicion_buffer);
		escribir_atributo_desde_int_de_buffer(buffer,
				&(pcb->indice_stack[i_stack]->posicion_variable_retorno->size),
				&posicion_buffer);

		int cant_argumentos;

		escribir_atributo_desde_int_de_buffer(buffer, &cant_argumentos,
				&posicion_buffer);
		if (cant_argumentos > 0) {
			pcb->indice_stack[i_stack]->argumentos = list_create();
			int i_argumentos;
			for (i_argumentos = 0; i_argumentos < cant_argumentos;
					++i_argumentos) {
				t_posicion_memoria *posicion_memoria = malloc(
						sizeof(t_posicion_memoria));

				escribir_atributo_desde_int_de_buffer(buffer,
						&(posicion_memoria->offset), &posicion_buffer);
				escribir_atributo_desde_int_de_buffer(buffer,
						&(posicion_memoria->pagina), &posicion_buffer);
				escribir_atributo_desde_int_de_buffer(buffer,
						&(posicion_memoria->size), &posicion_buffer);

				list_add(pcb->indice_stack[i_stack]->argumentos,
						posicion_memoria);
			}
		}
		int cant_variables;
		escribir_atributo_desde_int_de_buffer(buffer, &cant_variables,
				&posicion_buffer);
		if (cant_variables > 0) {
			pcb->indice_stack[i_stack]->variables = list_create();
			int i_variables;
			for (i_variables = 0; i_variables < cant_variables; ++i_variables) {

				t_variables_stack * variables_stack = malloc(
						sizeof(t_variables_stack));

				escribir_atributo_desde_int_de_buffer(buffer,
						&(variables_stack->id), &posicion_buffer);

				variables_stack->posicion_memoria = malloc(
						sizeof(t_posicion_memoria));

				escribir_atributo_desde_int_de_buffer(buffer,
						&(variables_stack->posicion_memoria->offset),
						&posicion_buffer);
				escribir_atributo_desde_int_de_buffer(buffer,
						&(variables_stack->posicion_memoria->pagina),
						&posicion_buffer);
				escribir_atributo_desde_int_de_buffer(buffer,
						&(variables_stack->posicion_memoria->size),
						&posicion_buffer);

				list_add(pcb->indice_stack[i_stack]->variables,
						variables_stack);

			}
		}
	}

}
