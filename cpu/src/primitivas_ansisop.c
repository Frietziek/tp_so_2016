/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "semaphore.h"
//#include "cpu.h"
#include <serializacion.h>
#include <comunicaciones.h>
#include "serializacion_cpu_umc.h"
#include "serializacion_cpu_nucleo.h"
#include "primitivas_ansisop.h"

#include "semaforo_sockets_cpu.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero ansisop_definir_variable(t_nombre_variable identificador_variable) {
	printf("Se define la variable %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_puntero ansisop_obtener_posicion_variable(
		t_nombre_variable identificador_variable) {
	printf("Obtiene la posicion de %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_valor_variable ansisop_derefenciar(t_puntero direccion_variable) {
	printf("Dereferencia %d y su valor es %i\n", direccion_variable,
			(int) valor_pagina);
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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_pagina);
	free(payload);

	printf("Espero semaforo\n");
	sem_wait(&s_pagina);
	printf("Signal recibido\n");
	return CONTENIDO_VARIABLE;
}

void ansisop_asignar(t_puntero direccion_variable, t_valor_variable valor) {
	printf("Asignando en %d el valor %d\n", direccion_variable, valor);

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

	t_buffer * payload = serializar_pagina_completa(p_pagina);

	header->longitud_mensaje = payload->longitud_buffer;

	if (enviar_buffer(socket_umc, header, payload)
			< sizeof(t_header) + payload->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_pagina);
	free(payload);
}

t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable) {
	printf("Obtiene valor de variable compartida %s\n", variable);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);

	// TODO Cambiar el return
	return CONTENIDO_VARIABLE;
}

t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	printf("Asignando en %s el valor %d\n", variable, valor);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);

	return CONTENIDO_VARIABLE;
}

void ansisop_ir_a_label(t_nombre_etiqueta etiqueta) {
	// TODO Terminar funcion
	printf("Yendo al Label: %s\n", etiqueta);
}

t_puntero_instruccion ansisop_llamar_funcion(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion) {
	// TODO Terminar funcion
	printf(
			"Llamando a la funcion de etiqueta %s, en el lugar de retorno %c y posicion de instruccion %c\n",
			etiqueta, donde_retornar, linea_en_ejecucion);
	return POSICION_MEMORIA;
}

void ansisop_retornar(t_valor_variable retorno) {
	// TODO Terminar funcion
	printf("Retornando el valor de la variable %d\n", retorno);
}

void ansisop_imprimir(t_valor_variable valor_mostrar) {
	printf("Imprimiendo el valor: %d\n", valor_mostrar);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_variable);
	free(p_buffer);
}

void ansisop_imprimir_texto(char* texto) {
	printf("Imprimiendo el texto: %s", texto);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_texto);
	free(p_buffer);
}

void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo) {
	printf("El dispositivo %s, tiene un tiempo de %d", dispositivo, tiempo);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_entrada_salida);
	free(p_buffer);
}

void ansisop_wait(t_nombre_semaforo semaforo) {
	printf("Esperando al semaforo %s", semaforo);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}

void ansisop_signal(t_nombre_semaforo semaforo) {
	printf("Semaforo activado %s", semaforo);

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
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}
