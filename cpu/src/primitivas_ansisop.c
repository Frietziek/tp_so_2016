/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "primitivas_ansisop.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero ansisop_definir_variable(t_nombre_variable variable) {
	log_info(logger_manager, "Se define la variable %c.", variable);

	return POSICION_MEMORIA;
}

t_puntero ansisop_obtener_posicion_variable(t_nombre_variable variable) {
	log_info(logger_manager, "La posicion de la variable es %c.", variable);

	return POSICION_MEMORIA;
}

t_valor_variable ansisop_derefenciar(t_puntero direccion_variable) {
	log_info(logger_manager, "Dereferencia de: %d.", direccion_variable);

	// TODO Rellenar con los valores reales
	int pagina = 4;
	int offset = 4;
	t_pagina *p_pagina = malloc(sizeof(t_pagina));
	p_pagina->pagina = pagina;
	p_pagina->offset = offset;
	p_pagina->tamanio = sizeof(int);
	p_pagina->socket_pedido = socket_umc;
	t_buffer *buffer = serializar_pagina(p_pagina);

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC, MENSAJE_LEER_PAGINA,
			"Fallo al enviar lectura de pagina a UMC.", buffer);

	free(p_pagina);
	free(buffer);

	sem_wait(&s_pagina);
	log_info(logger_manager, "Su valor es: %i.", valor_pagina);
	return CONTENIDO_VARIABLE;
}

void ansisop_asignar(t_puntero direccion, t_valor_variable valor) {
	log_info(logger_manager, "Asignando en: %d el valor: %i.", direccion,
			valor);

	// TODO Rellenar con los valores reales
	int pagina = 4;
	int offset = 4;

	t_pagina_completa *p_pagina = malloc(sizeof(t_pagina_completa));
	p_pagina->pagina = pagina;
	p_pagina->offset = offset;
	p_pagina->tamanio = sizeof(int);
	p_pagina->valor = (void*) valor;
	p_pagina->socket_pedido = socket_umc;
	t_buffer *buffer = serializar_pagina_completa(p_pagina);

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC, MENSAJE_ESCRIBIR_PAGINA,
			"Fallo al enviar escritura de pagina a UMC.", buffer);

	free(p_pagina);
	free(buffer);
}

t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable) {
	log_info(logger_manager, "El valor de variable compartida es %s.",
			variable);

	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;
	t_buffer *buffer = serializar_variable(p_compartida);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO,
	MENSAJE_LEER_COMPARTIDA, "Fallo al enviar lectura de compartida a Nucleo.",
			buffer);

	free(p_compartida);
	free(buffer);

	// TODO Cambiar el return
	return CONTENIDO_VARIABLE;
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

	free(p_compartida);
	free(buffer);

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

	t_variable_valor *p_variable = malloc(sizeof(t_variable_valor));
	p_variable->valor = valor_mostrar;
	t_buffer *buffer = serializar_variable_valor(p_variable);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO, MENSAJE_IMPRIMIR,
			"Fallo al enviar imprimir a Nucleo.", buffer);

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

	free(p_semaforo);
	free(buffer);
}

void ansisop_signal(t_nombre_semaforo semaforo) {
	log_info(logger_manager, "Activo al Semaforo: %s.", semaforo);

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;
	t_buffer *buffer = serializar_semaforo(p_semaforo);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO, MENSAJE_SIGNAL,
			"Fallo al enviar signal al Nucleo.", buffer);

	free(p_semaforo);
	free(buffer);
}
