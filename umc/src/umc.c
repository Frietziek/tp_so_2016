/*
 ============================================================================
 Name        : umc.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <comunicaciones.h>
#include <serializacion.h>
#include "umc.h"

#include "serializacion_umc_swap.h"
#include "deserializacion_cpu_umc.h"

int socket_swap;

int main(void) {
	int comando; // Comandos ingresados de la consola de UMC
	t_config_umc *configuracion = malloc(sizeof(t_config_umc)); // Estructura de configuracion de la UMC
	carga_configuracion_UMC("src/config.umc.ini", configuracion);

	// Se crea el bloque de la memoria principal
	/*void memoria_principal = calloc(configuracion->marcos,
	 configuracion->marco_size);*/

	// TODO Crear estructuras para programas
	// TODO Crear Cache TLB
	// Crear servidor (socket host)
	t_configuracion_servidor *servidor_umc = malloc(
			sizeof(t_configuracion_servidor));
	servidor_umc->puerto = configuracion->puerto;
	servidor_umc->funcion = &atender_peticiones;
	servidor_umc->parametros_funcion = configuracion;

	crear_servidor(servidor_umc);
	printf("Servidor UMC corriendo\n");

	//Se realiza una conexión con el swap (server)
	if ((socket_swap = conectar_servidor(configuracion->ip_swap,
			configuracion->puerto_swap)) > 0) {
		printf("UMC conectado con SWAP\n");
		handshake_umc_swap();
	} else {
		perror("Error al conectarse con el Swap");
	}

	// TODO Administrar los pedidos de las CPUs y el nucleo

	printf("Proceso UMC creado.\n");
	printf("Ingrese uno de los siguientes comandos para continuar:\n");
	printf("1 - Cambiar retardo de la consola UMC\n");
	printf("2 - Generar reporte y archivo Dump\n");
	printf("3 - Limpiar contenido de LTB o paginas\n");
	scanf("%d", &comando);
	// TODO Implementar funciones de la consola de UMC
	switch (comando) {
	case RETARDO:
		printf("Entro en Retardo\n");
		cambiar_retardo();
		break;
	case DUMP:
		printf("Entro en Dump\n");
		generar_dump();
		break;
	case TLB:
		printf("Entro en Flush de TLB\n");
		limpiar_contenido();
		break;
	default:
		printf("Comando no reconocido\n");
		break;
	}
	free(configuracion);
	return EXIT_SUCCESS;
}

void carga_configuracion_UMC(char *archivo, t_config_umc *configuracionUMC) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "PUERTO")) {
		configuracionUMC->puerto = config_get_int_value(configuracion,
				"PUERTO");
	}
	if (config_has_property(configuracion, "IP_SWAP")) {
		configuracionUMC->ip_swap = config_get_string_value(configuracion,
				"IP_SWAP");
	}
	if (config_has_property(configuracion, "PUERTO_SWAP")) {
		configuracionUMC->puerto_swap = config_get_int_value(configuracion,
				"PUERTO_SWAP");
	}
	if (config_has_property(configuracion, "MARCOS")) {
		configuracionUMC->marcos = config_get_int_value(configuracion,
				"MARCOS");
	}
	if (config_has_property(configuracion, "MARCO_SIZE")) {
		configuracionUMC->marco_size = config_get_int_value(configuracion,
				"MARCO_SIZE");
	}
	if (config_has_property(configuracion, "MARCO_X_PROC")) {
		configuracionUMC->marco_x_proc = config_get_int_value(configuracion,
				"MARCO_X_PROC");
	}
	if (config_has_property(configuracion, "ENTRADAS_TLB")) {
		configuracionUMC->entradas_tlb = config_get_int_value(configuracion,
				"ENTRADAS_TLB");
	}
	if (config_has_property(configuracion, "RETARDO")) {
		configuracionUMC->retardo = config_get_int_value(configuracion,
				"RETARDO");
	}
	free(configuracion);
}

void atender_peticiones(t_config_umc *config, t_paquete *paquete) {
	switch (paquete->header->id_proceso_emisor) {
	case PROCESO_CPU:
		atender_cpu(config, paquete);
		break;
	case PROCESO_NUCLEO:
		atender_nucleo(config, paquete);
		break;
	default:
		perror("No tiene permisos para comunicarse con la UMC\n");
		break;
	}
}

void atender_cpu(t_config_umc *config, t_paquete *paquete) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		printf("Handshake recibido de CPU\n");
		break;
	case MENSAJE_DEREFENCIAR:
		leer_pagina(paquete->payload);
		break;
	case MENSAJE_ASIGNAR_VARIABLE:
		escribir_pagina(paquete->payload);
		break;
	default:
		perror("Comando no reconocido\n");
		break;
	}
}

void atender_nucleo(t_config_umc *config, t_paquete *paquete) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		// TODO Responder con el tamanio de pagina
		break;
	default:
		perror("Comando no reconocido\n");
		break;
	}
}

/*void definir_variable() {
 // TODO Quien se encarga de guardar el espacio de la variable?
 // Como se en que pagina debo escribir?
 }*/

// Funciones UMC - Swap
void handshake_umc_swap() {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = PROCESO_SWAP;
	header->id_mensaje = MENSAJE_HANDSHAKE;
	header->longitud_mensaje = 0;

	enviar_header(socket_swap, header);
	free(header);
}
void inicializar_programa(void *buffer) {
// TODO Recibir mensaje de inicio de programa del nucleo
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = PROCESO_SWAP;
	header->id_mensaje = MENSAJE_INICIALIZAR_PROGRAMA;

	t_programa_completo *p_inicio_programa = malloc(
			sizeof(t_programa_completo));
// TODO Rellenar con lo que manda el Nucleo
	p_inicio_programa->id_programa = 10;
	p_inicio_programa->paginas_requeridas = 100;
	t_buffer *p_buffer = serializar_inicio_programa(p_inicio_programa);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_swap, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_inicio_programa);
	free(p_buffer);
// TODO Responder al nucleo
}

void leer_pagina(void *buffer) {
// TODO Recibir pedido del cpu
// TODO Verificar si esta en memoria, si no hacer el pedido al swap
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = PROCESO_SWAP;
	header->id_mensaje = MENSAJE_LEER_PAGINA;

	t_pagina *p_pagina = malloc(sizeof(t_pagina));
// TODO Rellenar con lo que manda el CPU
	p_pagina->pagina = 10;
	p_pagina->offset = 2;
	p_pagina->tamanio = 4;
	t_buffer *p_buffer = serializar_leer_pagina(p_pagina);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_swap, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_pagina);
	free(p_buffer);
// TODO Responder el pedido a la CPU
}

void escribir_pagina(void *buffer) {
// TODO Recibir pedido del cpu
// TODO Verificar si esta en memoria, si no hacer el pedido al swap
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = PROCESO_SWAP;
	header->id_mensaje = MENSAJE_ESCRIBIR_PAGINA;

	t_pagina_completa *p_pagina = malloc(sizeof(t_pagina_completa));
// TODO Rellenar con lo que manda el CPU
	p_pagina->pagina = 10;
	p_pagina->offset = 2;
	p_pagina->tamanio = 4;
	p_pagina->valor = 20;
	t_buffer *p_buffer = serializar_escribir_pagina(p_pagina);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_swap, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_pagina);
	free(p_buffer);
// TODO Responder el pedido a la CPU
}

void finalizar_programa(void *buffer) {
// TODO Recibir mensaje de fin de programa del nucleo
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = PROCESO_SWAP;
	header->id_mensaje = MENSAJE_FINALIZAR_PROGRAMA;

	t_programa *p_fin_programa = malloc(sizeof(t_programa));
// TODO Rellenar con lo que manda el Nucleo
	p_fin_programa->id_programa = 10;
	t_buffer *p_buffer = serializar_finalizar_programa(p_fin_programa);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_swap, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_fin_programa);
	free(p_buffer);
// TODO Responder al nucleo
}

void cambiar_retardo() {
// TODO Terminar funcion
}

void generar_dump() {
// TODO Terminar funcion
}

void limpiar_contenido() {
// TODO Terminar funcion
}

