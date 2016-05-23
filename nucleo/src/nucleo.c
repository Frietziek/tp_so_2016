/*
 ============================================================================
 Name        : nucleo.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <serializacion.h>
#include <comunicaciones.h>
#include "nucleo.h"
#include <commons/config.h>
#include <parser/metadata_program.h>
#include "serializacion_nucleo_consola.h"
#include "serializacion_nucleo_cpu.h"

//TODO ver si quedan como variables globales o no
t_queue *cola_ready;
t_queue *cola_block;
t_queue *cola_exec;
t_queue *cola_exit;

int main(void) {

	puts("Hola soy el nucleo"); /* prints proceso */

	t_config_nucleo *configuracion = malloc(sizeof(t_config_nucleo));
	cargarConfiguracionNucleo("src/config.nucleo.ini", configuracion);

	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();

//	int tamanio_pagina = conectar_umc_y_obtener_tamanio_pagina(configuracion);
//
//	printf("el tamaño de pagina es: %d\n\n", tamanio_pagina);

	t_configuracion_servidor *configuracion_servidor = malloc(
			sizeof(t_configuracion_servidor));

	configuracion_servidor->puerto = configuracion->puerto_prog;

	//ESTO ES LO QUE HAY QUE HACER, EN configuracion_servidor->funcion  PONER &nombre_de_funcion
	//Y EN configuracion_servidor->parametros_funcion LO QUE DEBERIA RECIBIR LA FUNCION
	configuracion_servidor->funcion = &atender_cpu;
	configuracion_servidor->parametros_funcion = configuracion;

	crear_servidor(configuracion_servidor);

	getchar();
	free(configuracion);
	free(configuracion_servidor);
	queue_destroy(cola_block);
	queue_destroy(cola_ready);
	queue_destroy(cola_exec);
	queue_destroy(cola_exit);

	return EXIT_SUCCESS;
}

void obtener_tamanio_pagina(int *tamanio_pagina, void *buffer) {

	//TODO deserializar mensaje umc
	//TODO asignar a lo que apunta el puntero tamanio_pagina el valor que esta en el payload del buffer
	*tamanio_pagina = 5;
}

//TODO llegado el momento hay que evaluar que devuelva correctamente el valor
int conectar_umc_y_obtener_tamanio_pagina(t_config_nucleo* configuracion) {
	int socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc);
	enviar_mensaje(socket_umc, "Hola soy el nucleo");

	t_th_parametros_receive *parametros = malloc(
			sizeof(t_th_parametros_receive));
	int tamanio_pagina;
	parametros->funcion = &obtener_tamanio_pagina;
	parametros->parametros_funcion = &tamanio_pagina;

	recibir_mensaje(parametros);

	return (int) &(parametros->parametros_funcion);
}

void atender_cpu(t_config_nucleo*config, void *buffer) {

	t_header *header = malloc(sizeof(t_header));

	deserializar_header(buffer, header);

	switch (header->id_mensaje) {
	case HANDSHAKE:
		printf("Empieza handshake\n\n");
		printf("Se establecio conexion con cpu\n\n");
		printf("proceso emisor: %d\n", header->id_proceso_emisor);
		printf("proceso receptor: %d\n", header->id_proceso_receptor);
		printf("id mensaje: %d\n", header->id_mensaje);
		printf("longitud payload: %d\n\n", header->longitud_mensaje);
		printf("termina handshake\n\n");
		break;
	case RECIBIR_PERSONA:

		printf(
				"hola mundo, soy el Reno Jose, el puerto del cpu es %d, el primer semaforo es: %s\n\n",
				config->puerto_cpu, config->sem_id[0]);

		//printf("hola mundo, soy el Reno Jose, el buffer contiene: %s \n", buffer);

		t_persona *persona2 = malloc(sizeof(t_persona));

		t_paquete *paquete = deserializar_con_header(buffer);

		deserializar_persona(paquete->payload, persona2);

		printf("el apellido de la persona es: %s\n", persona2->apellido);

		printf("el cp de la persona es: %d\n", persona2->cp);

		printf("la edad de la persona es: %d\n", persona2->edad);

		printf("el nombre de la persona es: %s\n", persona2->nombre);

		printf("la cant de materias aprobadas es: %d\n\n",
				persona2->materias_aprobadas);

		//TODO ver por que el proceso emisor es 0 en vez de 2
		printf("proceso emisor: %d\n", paquete->header->id_proceso_emisor);
		printf("proceso receptor: %d\n", paquete->header->id_proceso_receptor);
		printf("id mensaje: %d\n", paquete->header->id_mensaje);
		printf("longitud payload: %d\n\n", paquete->header->longitud_mensaje);

		free(persona2);
		free(paquete);
		break;
	}
	free(header);
}

void cargarConfiguracionNucleo(char *archivoConfig,
		t_config_nucleo *configuracion_nucleo) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivoConfig);
	if (config_has_property(configuracion, "DEF_PUERTO_PROG")) {
		configuracion_nucleo->puerto_prog = config_get_int_value(configuracion,
				"DEF_PUERTO_PROG");
	} else {
		perror("error al cargar DEF_PUERTO_PROG");
	}
	if (config_has_property(configuracion, "DEF_PUERTO_CPU")) {
		configuracion_nucleo->puerto_cpu = config_get_int_value(configuracion,
				"DEF_PUERTO_CPU");
	} else {
		perror("error al cargar DEF_PUERTO_CPU");
	}
	if (config_has_property(configuracion, "DEF_QUANTUM")) {
		configuracion_nucleo->quantum = config_get_int_value(configuracion,
				"DEF_QUANTUM");
	} else {
		perror("error al cargar DEF_QUANTUM");
	}
	if (config_has_property(configuracion, "DEF_QUANTUM_SLEEP")) {
		configuracion_nucleo->quantum_sleep = config_get_int_value(
				configuracion, "DEF_QUANTUM_SLEEP");
	} else {
		perror("error al cargar DEF_QUANTUM_SLEEP");
	}

	if (config_has_property(configuracion, "DEF_IO_ID")) {
		configuracion_nucleo->io_id = config_get_array_value(configuracion,
				"DEF_IO_ID");
	} else {
		perror("error al cargar DEF_IO_ID");
	}
	if (config_has_property(configuracion, "DEF_IO_SLEEP")) {
		configuracion_nucleo->io_sleep = config_get_array_value(configuracion,
				"DEF_IO_SLEEP");
	} else {
		perror("error al cargar DEF_IO_SLEEP");
	}

	if (config_has_property(configuracion, "DEF_SEM_IDS")) {
		configuracion_nucleo->sem_id = config_get_array_value(configuracion,
				"DEF_SEM_IDS");
	} else {
		perror("error al cargar DEF_SEM_IDS");
	}

	if (config_has_property(configuracion, "DEF_SEM_INIT")) {
		configuracion_nucleo->sem_init = config_get_array_value(configuracion,
				"DEF_SEM_INIT");
	} else {
		perror("error al cargar DEF_SEM_INIT");
	}

	if (config_has_property(configuracion, "DEF_SHARED_VARS")) {
		configuracion_nucleo->shared_vars = config_get_array_value(
				configuracion, "DEF_SHARED_VARS");
	} else {
		perror("error al cargar DEF_SHARED_VARS");
	}

	if (config_has_property(configuracion, "DEF_IP_UMC")) {
		configuracion_nucleo->ip_umc = config_get_string_value(configuracion,
				"DEF_IP_UMC");
	} else {
		perror("error al cargar DEF_IP_UMC");
	}

	if (config_has_property(configuracion, "DEF_PUERTO_UMC")) {
		configuracion_nucleo->puerto_umc = config_get_int_value(configuracion,
				"DEF_PUERTO_UMC");
	} else {
		perror("error al cargar DEF_PUERTO_UMC");
	}
	if (config_has_property(configuracion, "DEF_STACK_SIZE")) {
		configuracion_nucleo->stack_size = config_get_int_value(configuracion,
				"DEF_STACK_SIZE");
	} else {
		perror("error al cargar DEF_STACK_SIZE");
	}

	free(configuracion);
}

void atender_consola(void *buffer) {

	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete = deserializar_con_header(buffer);
	char *codigo_de_consola;
	switch (paquete->header->id_mensaje) {
	case CODIGO:
		deserializar_codigo(paquete->payload, &codigo_de_consola,
				paquete->header->longitud_mensaje);
		printf("el codigo es: %s\n", codigo_de_consola);
		t_pcb *pcb = crearPCB(codigo_de_consola);
		agregar_pcb_a_cola(cola_ready, pcb);
		//libero
		free(pcb);
		free(codigo_de_consola);
		break;
	case FINALIZAR:
		printf("Terminando la ejecucion");
		terminar_ejecucion();
		printf("Termino la ejecucion");
		break;
	}
	free(paquete);
}

t_pcb *crearPCB(char *codigo_de_consola) {
	t_pcb *pcb = malloc(sizeof(t_pcb));
//TODO usar parser metadata para llenar el pcb

	return pcb;
}

void agregar_pcb_a_cola(t_queue *cola, t_pcb *pcb) {
	queue_push(cola, pcb);
}

//TODO ver como identificar el proceso para terminarlo, podria ser con el pid
void terminar_ejecucion() {
}
