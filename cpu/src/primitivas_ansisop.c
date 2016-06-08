/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "primitivas_ansisop.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero ansisop_definir_variable(t_nombre_variable identificador_variable) {
	log_info(logger_manager, "Se define la variable %c.",
			identificador_variable);

	return POSICION_MEMORIA;
}

t_puntero ansisop_obtener_posicion_variable(
		t_nombre_variable identificador_variable) {
	log_info(logger_manager, "Se obtiene la posicion de la variable %c.",
			identificador_variable);

	return POSICION_MEMORIA;
}

t_valor_variable ansisop_derefenciar(t_puntero direccion_variable) {
	log_info(logger_manager, "Dereferencia de: %d.", direccion_variable);
	// TODO Rellenar con los valores reales
	int pagina = 4;
	int offset = 4;

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_UMC;
	header->id_mensaje = MENSAJE_LEER_PAGINA;

	t_pagina *p_pagina = malloc(sizeof(t_pagina));
	p_pagina->pagina = pagina;
	p_pagina->offset = offset;
	p_pagina->tamanio = sizeof(int);
	p_pagina->socket_pedido = socket_umc;
	t_buffer *payload = serializar_pagina(p_pagina);

	header->longitud_mensaje = payload->longitud_buffer;

	if (enviar_buffer(socket_umc, header, payload)
			< sizeof(t_header) + payload->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar lectura de pagina a UMC.");
	}

	free(header);
	free(p_pagina);
	free(payload);

	sem_wait(&s_pagina);
	log_info(logger_manager, "Su valor es: %i.", valor_pagina);
	return CONTENIDO_VARIABLE;
}

void ansisop_asignar(t_puntero direccion_variable, t_valor_variable valor) {
	log_info(logger_manager, "Asignando en: %d el valor: %i.",
			direccion_variable, valor);
	// TODO Rellenar con los valores reales
	int pagina = 4;
	int offset = 4;

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_UMC;
	header->id_mensaje = MENSAJE_ESCRIBIR_PAGINA;

	t_pagina_completa *p_pagina = malloc(sizeof(t_pagina_completa));
	p_pagina->pagina = pagina;
	p_pagina->offset = offset;
	p_pagina->tamanio = sizeof(int);
	p_pagina->valor = valor;
	p_pagina->socket_pedido = socket_umc;

	t_buffer *payload = serializar_pagina_completa(p_pagina);

	header->longitud_mensaje = payload->longitud_buffer;

	if (enviar_buffer(socket_umc, header, payload)
			< sizeof(t_header) + payload->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar escritura de pagina a UMC.");
	}

	free(header);
	free(p_pagina);
	free(payload);
}

t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable) {
	log_info(logger_manager, "Obtengo el valor de variable compartida: %s.",
			variable);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_OBTENER_VALOR_COMPARTIDA;

	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;

	t_buffer * p_buffer = serializar_variable(p_compartida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager,
				"Fallo al enviar lectura de compartida a Nucleo.");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);

	// TODO Cambiar el return
	return CONTENIDO_VARIABLE;
}

t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	log_info(logger_manager, "Asigno en variable compartida: %s el valor %d.",
			variable, valor);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA;

	t_variable_completa *p_compartida = malloc(sizeof(t_variable_completa));
	p_compartida->nombre = variable;
	p_compartida->valor = valor;

	t_buffer * p_buffer = serializar_variable_completa(p_compartida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager,
				"Fallo al enviar escritura de compartida a Nucleo.");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);

	return CONTENIDO_VARIABLE;
}

void ansisop_ir_a_label(t_nombre_etiqueta etiqueta) {
	// TODO Terminar funcion
	log_info(logger_manager, "Voy a la etiqueta: %s.", etiqueta);
}

t_puntero_instruccion ansisop_llamar_funcion(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion) {
	// TODO Terminar funcion
	log_info(logger_manager,
			"Llamo a funcion de etiqueta: %s en retorno: %c y posicion de instruccion %c.",
			etiqueta, donde_retornar, linea_en_ejecucion);
	return POSICION_MEMORIA;
}

void ansisop_retornar(t_valor_variable retorno) {
	// TODO Terminar funcion
	log_info(logger_manager, "Retorno el valor de la variable %d.", retorno);
}

void ansisop_imprimir(t_valor_variable valor_mostrar) {
	log_info(logger_manager, "Imprimo el valor: %d.", valor_mostrar);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_IMPRIMIR;

	t_variable_valor *p_variable = malloc(sizeof(t_variable_valor));
	p_variable->valor = valor_mostrar;

	t_buffer * p_buffer = serializar_variable_valor(p_variable);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar imprimir a Nucleo.");
	}

	free(header);
	free(p_variable);
	free(p_buffer);
}

void ansisop_imprimir_texto(char* texto) {
	log_info(logger_manager, "Imprimo el texto: %s.", texto);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_IMPRIMIR_TEXTO;

	t_texto *p_texto = malloc(sizeof(t_texto));
	p_texto->texto = texto;

	t_buffer * p_buffer = serializar_texto(p_texto);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar imprimir texto a Nucleo.");
	}

	free(header);
	free(p_texto);
	free(p_buffer);
}

void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo) {
	log_info(logger_manager, "I/O con dispositivo: %s  y tiempo: %d.",
			dispositivo, tiempo);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_ENTRADA_SALIDA;

	t_entrada_salida *p_entrada_salida = malloc(sizeof(t_entrada_salida));
	p_entrada_salida->nombre_dispositivo = dispositivo;
	p_entrada_salida->tiempo = tiempo;

	t_buffer * p_buffer = serializar_entrada_salida(p_entrada_salida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar I/O a Nucleo.");
	}

	free(header);
	free(p_entrada_salida);
	free(p_buffer);
}

void ansisop_wait(t_nombre_semaforo semaforo) {
	log_info(logger_manager, "Esperando al semaforo: %s.", semaforo);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_WAIT;

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;

	t_buffer * p_buffer = serializar_semaforo(p_semaforo);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar wait a Nucleo.");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}

void ansisop_signal(t_nombre_semaforo semaforo) {
	log_info(logger_manager, "Semaforo activado: %s.", semaforo);

	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_SIGNAL;

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;

	t_buffer * p_buffer = serializar_semaforo(p_semaforo);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		log_error(logger_manager, "Fallo al enviar signal a Nucleo.");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}
