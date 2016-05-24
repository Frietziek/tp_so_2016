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

int tamanio_pagina;

int main(void) {

	puts("Hola soy el nucleo"); /* prints proceso */

	t_config_nucleo *configuracion = malloc(sizeof(t_config_nucleo));
	cargarConfiguracionNucleo("src/config.nucleo.ini", configuracion);

	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();

	//TODO inicializar colas_de_cada_dispositivo_entrada_salida

	// INICIO PIDO LONGITUD PAGINA  AL UMC Y LO ATIENDO

	int socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc);

	t_header *header_pido_tam_pag = malloc(sizeof(t_header));

	header_pido_tam_pag->id_proceso_emisor = 1;
	header_pido_tam_pag->id_proceso_receptor = 3;
	//TODO definir id mensaje para pedir tamaño pagina
	header_pido_tam_pag->id_mensaje = 0;
	header_pido_tam_pag->longitud_mensaje = 0;

	enviar_header(socket_umc, header_pido_tam_pag);

	//Creamos estructura para mandarsela a recv
	t_th_parametros_receive *parametros_receive_umc = malloc(
			sizeof(t_th_parametros_receive));

	parametros_receive_umc->funcion = &obtener_tamanio_pagina;

	parametros_receive_umc->socket_cliente = socket_umc;

	//recibimos tam pagina
	recibir_mensaje(parametros_receive_umc);

	// FIN PIDO LONGITUD PAGINA  AL UMC Y LO ATIENDO

	//INICIO ATIENDO CONSOLA

	t_configuracion_servidor *configuracion_servidor_consola = malloc(
			sizeof(t_configuracion_servidor));

	configuracion_servidor_consola->puerto = configuracion->puerto_prog;

	//ESTO ES LO QUE HAY QUE HACER, EN configuracion_servidor->funcion  PONER &nombre_de_funcion

	configuracion_servidor_consola->funcion = &atender_consola;


	crear_servidor(configuracion_servidor_consola);

	//FIN ATIENDO CONSOLA

	//INICIO ATIENDO CPU

	t_configuracion_servidor *configuracion_servidor_cpu = malloc(
			sizeof(t_configuracion_servidor));

	configuracion_servidor_cpu->puerto = configuracion->puerto_cpu;

	//ESTO ES LO QUE HAY QUE HACER, EN configuracion_servidor->funcion  PONER &nombre_de_funcion
	configuracion_servidor_cpu->funcion = &atender_cpu;


	crear_servidor(configuracion_servidor_cpu);

	//FIN ATIENDO CPU

//	TODO while(true){
//	if(hay programas por ejecutar y hay cpu conectada){
//		planifico los programas de la consola con round robin
//	};
//
//	};

	getchar();

	//Libero antes de cerrar
	free(configuracion);
	free(configuracion_servidor_cpu);
	free(header_pido_tam_pag);
	free(parametros_receive_umc);
	free(configuracion_servidor_consola);

	queue_destroy(cola_block);
	queue_destroy(cola_ready);
	queue_destroy(cola_exec);
	queue_destroy(cola_exit);

	return EXIT_SUCCESS;
}

void obtener_tamanio_pagina(t_paquete *paquete) {

	//TODO falta definir serializacion umc nucleo
	//TODO deserializar mensaje umc
	//TODO asignar el contenido de paquete->payload a tamanio pagina
	tamanio_pagina = 5;

}

void atender_cpu(t_paquete *paquete, int socket_cpu) {

	//INICIO EJEMPLO CONSUMIR HEADER DE PAQUETE

	printf("proceso emisor: %d\n", paquete->header->id_proceso_emisor);
	printf("proceso receptor: %d\n", paquete->header->id_proceso_receptor);
	printf("id mensaje: %d\n", paquete->header->id_mensaje);
	printf("longitud payload: %d\n\n", paquete->header->longitud_mensaje);

	//FIN EJEMPLO CONSUMIR HEADER DE PAQUETE

	printf("el socket del cpu es: %d\n", socket_cpu);

	//INICIO EJEMPLO CONSUMIR PAYLOAD O NO SEGUN ID MENSAJE
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		printf("Empieza handshake\n\n");
		printf("Se establecio conexion con cpu\n\n");
		printf("termina handshake\n\n");

		break;
	case RECIBIR_PERSONA:

		//INICIO EJEMPLO CONSUMIR PAYLOAD

		; // el ; es necesario para poder hacer t_persona *persona2 = malloc(sizeof(t_persona));

		t_persona *persona2 = malloc(sizeof(t_persona));

		deserializar_persona(paquete->payload, persona2);

		printf("el apellido de la persona es: %s\n", persona2->apellido);

		printf("el cp de la persona es: %d\n", persona2->cp);

		printf("la edad de la persona es: %d\n", persona2->edad);

		printf("el nombre de la persona es: %s\n", persona2->nombre);

		printf("la cant de materias aprobadas es: %d\n\n",
				persona2->materias_aprobadas);

		free(persona2);
		break;
		//FIN EJEMPLO CONSUMIR PAYLOAD
	}
	//FIN EJEMPLO CONSUMIR PAYLOAD O NO SEGUN ID MENSAJE

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

void atender_consola(t_paquete *paquete_buffer) {

	printf("proceso emisor: %d\n", paquete_buffer->header->id_proceso_emisor);
	printf("proceso receptor: %d\n",
			paquete_buffer->header->id_proceso_receptor);
	printf("id mensaje: %d\n", paquete_buffer->header->id_mensaje);
	printf("longitud payload: %d\n\n",
			paquete_buffer->header->longitud_mensaje);

	switch (paquete_buffer->header->id_mensaje) {
	case CODIGO:
		;
		//inicio para ver lo que contiene el payload
		char *codigo_de_consola = malloc(
				paquete_buffer->header->longitud_mensaje + 1);
		deserializar_codigo(paquete_buffer->payload, &codigo_de_consola,
				paquete_buffer->header->longitud_mensaje);
		codigo_de_consola[paquete_buffer->header->longitud_mensaje] = '\0';
		printf("el codigo es: %s\n\n", codigo_de_consola);
		free(codigo_de_consola);
		//fin para ver lo que contiene el payload

		t_pcb *pcb = crearPCB(codigo_de_consola);
		agregar_pcb_a_cola(cola_ready, pcb);
		//libero
		free(pcb);

		break;
	case FINALIZAR:
		printf("Terminando la ejecucion");
		terminar_ejecucion();
		printf("Termino la ejecucion");
		break;
	}
	free(paquete_buffer);
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
