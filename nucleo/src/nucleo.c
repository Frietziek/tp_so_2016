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
#include "serializacion_nucleo_consola.h"
#include "serializacion_nucleo_cpu.h"
#include "serializacion_nucleo_umc.h"
#include "semaforos_nucleo.h"
#include "atiendo_cpu.h"

//TODO acomodar este mounstruo

////////////////////VARIABLES GLOBALES///////////////////////////

t_queue *cola_new;
t_queue *cola_ready;
t_queue *cola_block;
t_queue *cola_exec;
t_queue *cola_exit;
t_list *lista_entrada_salida;
int tamanio_pagina;
int socket_umc;
int pid_count;
t_fila_tabla_procesos *tabla_procesos[];
t_config_nucleo *configuracion;

////////////////////FUNCION PRINCIPAL///////////////////////////

int main(void) {

	puts("Hola soy el nucleo"); /* prints proceso */
	pid_count = 0;
	configuracion = malloc(sizeof(t_config_nucleo));
	cargar_configuracion_nucleo("src/config.nucleo.ini", configuracion);

	cola_new = queue_create();
	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();

	sem_init(&mutex_pid_count, 0, 1);
	sem_init(&mutex_cola_ready, 0, 1);
	sem_init(&mutex_cola_exec, 0, 1);
	sem_init(&mutex_cola_block, 0, 1);
	sem_init(&mutex_tabla_procesos, 0, 1);
//TODO sem init  tabla io

	inicializar_colas_entrada_salida(configuracion->io_id,
			configuracion->io_sleep);
	// INICIO PIDO LONGITUD PAGINA  AL UMC Y LO ATIENDO

	socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc, &atender_umc);
	pedir_pagina_tamanio(socket_umc);

	// FIN PIDO LONGITUD PAGINA  AL UMC Y LO ATIENDO

	//INICIO ATIENDO CONSOLA

	t_configuracion_servidor *configuracion_servidor_consola = malloc(
			sizeof(t_configuracion_servidor));
	configuracion_servidor_consola->puerto = configuracion->puerto_prog;
	configuracion_servidor_consola->funcion = &atender_consola;

	crear_servidor(configuracion_servidor_consola);

	//FIN ATIENDO CONSOLA

	//INICIO ATIENDO CPU

	t_configuracion_servidor *configuracion_servidor_cpu = malloc(
			sizeof(t_configuracion_servidor));
	configuracion_servidor_cpu->puerto = configuracion->puerto_cpu;
	configuracion_servidor_cpu->funcion = &atender_cpu;
	configuracion_servidor_cpu->parametros_funcion = configuracion;

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
	free(configuracion_servidor_consola);

	queue_destroy(cola_block);
	queue_destroy(cola_ready);
	queue_destroy(cola_exec);
	queue_destroy(cola_exit);

	sem_destroy(&mutex_pid_count);
	sem_destroy(&mutex_cola_block);
	sem_destroy(&mutex_cola_ready);
	sem_destroy(&mutex_cola_exec);

	return EXIT_SUCCESS;
}

////////////////////FUNCIONES ATENDER///////////////////////////

void atender_umc(t_paquete *paquete, int socket_conexion) {

	printf("proceso emisor: %d\n", paquete->header->id_proceso_emisor);
	printf("proceso receptor: %d\n", paquete->header->id_proceso_receptor);
	printf("id mensaje: %d\n", paquete->header->id_mensaje);
	printf("longitud payload: %d\n\n", paquete->header->longitud_mensaje);

	switch (paquete->header->id_mensaje) {
	case RECIBIR_TAMANIO_PAGINA:
		;
		t_pagina_tamanio *pagina = malloc(sizeof(t_pagina_tamanio));
		deserializar_pagina_tamanio(paquete->payload, pagina);
		tamanio_pagina = pagina->tamanio;
		printf("Se cargo el tamanio de la pagina: %i\n", tamanio_pagina);
		free(pagina);
		break;
	case RESPUESTA_INICIALIZAR_PROGRAMA:
		//TODO
		//deserializo el pid del proceso iniciado y lo agrego a la cola de ready
		break;
	case RESPUESTA_MATAR_PROGRAMA:
		//TODO
		//avisar a la consola que se cerro umc y liberar t0do lo del proceso
		break;
	case ERROR_INICIALIZAR_PROGRAMA:
		//TODO
		//avisar a la consola y liberar t0do lo del proceso
		break;
	case ERROR_MATAR_PROGRAMA:
		//TODO
		//avisar a la consola que no se pudo cerrar umc y liberar t0do lo del proceso
		break;
	}
}

void atender_consola(t_paquete *paquete_buffer, int socket_consola) {

	printf("proceso emisor: %d\n", paquete_buffer->header->id_proceso_emisor);
	printf("proceso receptor: %d\n",
			paquete_buffer->header->id_proceso_receptor);
	printf("id mensaje: %d\n", paquete_buffer->header->id_mensaje);
	printf("longitud payload: %d\n\n",
			paquete_buffer->header->longitud_mensaje);

	switch (paquete_buffer->header->id_mensaje) {
	case CODIGO:
		;
		//inicio para ver lo que contiene el payload,
		t_texto *codigo_de_consola = malloc(sizeof(t_texto));
		//TODO ver si esta bien
		deserializar_codigo(paquete_buffer->payload, codigo_de_consola);
		char *codigo = malloc(paquete_buffer->header->longitud_mensaje + 1);
		memcpy(codigo, codigo_de_consola->texto,
				paquete_buffer->header->longitud_mensaje);
		codigo[paquete_buffer->header->longitud_mensaje] = '\0';
		printf("el codigo es:\n %s\n\n", codigo);

		//fin para ver lo que contiene el payload,

		t_pcb *pcb = crearPCB(codigo_de_consola->texto);

		agregar_pcb_a_tabla_procesos(pcb, socket_consola);

		enviar_programa_completo_a_umc(pcb->pid, pcb->cant_paginas_codigo_stack,
				codigo);
		free(codigo_de_consola);
		free(codigo);
		break;
	case FINALIZAR:
		printf("Terminando la ejecucion");
		t_pcb *pcb_obtenido = buscar_pcb_por_socket_consola(socket_consola);
		terminar_ejecucion(pcb_obtenido);
		printf("Termino la ejecucion");
		break;
	}

	free(paquete_buffer);
}

////////////////////FUNCIONES AUXILIARES///////////////////////////

t_pcb *buscar_pcb_por_socket_consola(int socket_consola) {

	sem_wait(&mutex_tabla_procesos);
	int cant_filas = sizeof(*tabla_procesos) / sizeof(t_fila_tabla_procesos);
	int i;
	for (i = 0; i < cant_filas; ++i) {
		if (tabla_procesos[i]->socket_consola == socket_consola) {
			sem_post(&mutex_tabla_procesos);
			return tabla_procesos[i]->pcb;
		} else {
			++i;
		}
	}
	sem_post(&mutex_tabla_procesos);
	return NULL;

}

void enviar_programa_completo_a_umc(int pid, int cant_paginas_codigo_stack,
		char *codigo_consola) {

	t_programa_completo *programa = malloc(sizeof(t_programa_completo));
	programa->id_programa = pid;
	programa->paginas_requeridas = cant_paginas_codigo_stack;
	programa->codigo = codigo_consola;

	t_buffer *payload_programa_completo = serializar_programa_completo(
			programa);

	t_header *header_prog_completo = malloc(sizeof(t_header));
	header_prog_completo->id_proceso_emisor = PROCESO_NUCLEO;
	header_prog_completo->id_proceso_receptor = PROCESO_UMC;
	header_prog_completo->id_mensaje = MENSAJE_INICIALIZAR_PROGRAMA;
	header_prog_completo->longitud_mensaje =
			payload_programa_completo->longitud_buffer;

	if (enviar_buffer(socket_umc, header_prog_completo,
			payload_programa_completo)
			< sizeof(header_prog_completo)
					+ payload_programa_completo->longitud_buffer) {
		perror("Fallo al inicializar el programa");
	}

	free(programa);
	free(header_prog_completo);
	free(payload_programa_completo);

}

void agregar_pcb_a_tabla_procesos(t_pcb *pcb, int socket_consola) {
	t_fila_tabla_procesos *fila = malloc(sizeof(t_fila_tabla_procesos));
	fila->pcb = pcb;
	fila->socket_consola = socket_consola;
	fila->socket_cpu = NO_ASIGNADO;
	sem_wait(&mutex_tabla_procesos);
	tabla_procesos[pcb->pid] = fila;
	sem_post(&mutex_tabla_procesos);
}

//TODO ver si debe ser un char con strlen o el tamanio es sin el \0
int obtener_cantidad_paginas_codigo_stack(char *codigo_de_consola) {
	int modulo = (strlen(codigo_de_consola) + configuracion->stack_size)
			% tamanio_pagina;
	int division = (strlen(codigo_de_consola) + configuracion->stack_size)
			/ tamanio_pagina;
	if (modulo == 0) {
		return division;
	} else
		return division + 1;
}

t_pcb *crearPCB(char *codigo_de_consola) {
	t_metadata_program *metadata = malloc(sizeof(t_metadata_program));
	metadata = metadata_desde_literal(codigo_de_consola);
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->instrucciones_serializadas = metadata->instrucciones_serializado;
	pcb->instrucciones_size = metadata->instrucciones_size;
	pcb->estado = NEW;
	pcb->cant_paginas_codigo_stack = obtener_cantidad_paginas_codigo_stack(
			codigo_de_consola);
	pcb->pc = 0;
	pcb->etiquetas = metadata->etiquetas;
	pcb->etiquetas_size = metadata->etiquetas_size;
	pcb->stack_size = configuracion->stack_size;
	//TODO Ver con Lea
	pcb->stack_position = strlen(codigo_de_consola);
	sem_wait(&mutex_pid_count);
	pcb->pid = pid_count;
	++pid_count;
	sem_post(&mutex_pid_count);

	//falta estructura stack

	return pcb;
}

void terminar_ejecucion(t_pcb *pcb_a_finalizar) {

	if (pcb_a_finalizar->estado != EXIT) {

		// ENVIO TERMINAR AL CPU

		t_finalizar *finalizar = malloc(sizeof(t_finalizar));

		finalizar->pid = pcb_a_finalizar->pid;

		t_buffer *buffer_finalizar = serializar_finalizar(finalizar);

		if (pcb_a_finalizar->estado == EXEC) {

			t_header *header_finalizar_cpu = malloc(sizeof(t_header));
			header_finalizar_cpu->id_proceso_emisor = PROCESO_NUCLEO;
			header_finalizar_cpu->id_proceso_receptor = PROCESO_CPU;
			header_finalizar_cpu->id_mensaje = FINALIZAR;
			header_finalizar_cpu->longitud_mensaje = 0;

			int socket_cpu = buscar_socket_cpu_por_pcb(pcb_a_finalizar);

			if (enviar_header(socket_cpu, header_finalizar_cpu)
					< sizeof(t_header)) {
				perror("Fallo enviar finalizar a cpu");
			}
			free(header_finalizar_cpu);
		}

		// ENVIO TERMINAR AL UMC

		t_header *header_finalizar_umc = malloc(sizeof(t_header));

		header_finalizar_umc->id_proceso_emisor = PROCESO_NUCLEO;
		header_finalizar_umc->id_proceso_receptor = PROCESO_UMC;
		header_finalizar_umc->id_mensaje = MENSAJE_MATAR_PROGRAMA;
		header_finalizar_umc->longitud_mensaje =
				buffer_finalizar->longitud_buffer;

		if (enviar_buffer(socket_umc, header_finalizar_umc, buffer_finalizar)
				< sizeof(t_header) + buffer_finalizar->longitud_buffer) {
			perror("Fallo enviar buffer finalizar umc");
		}

		free(finalizar);
		free(buffer_finalizar);
		free(header_finalizar_umc);
	}
}

int buscar_socket_cpu_por_pcb(t_pcb *pcb_a_finalizar) {
	sem_wait(&mutex_tabla_procesos);
	int cant_filas = sizeof(*tabla_procesos) / sizeof(t_fila_tabla_procesos);
	int i;
	for (i = 0; i < cant_filas; ++i) {
		if (tabla_procesos[i]->pcb->pid == pcb_a_finalizar->pid) {
			sem_post(&mutex_tabla_procesos);
			return tabla_procesos[i]->socket_cpu;
		} else {
			++i;
		}
	}
	sem_post(&mutex_tabla_procesos);
	return NO_ASIGNADO;
}

void inicializar_colas_entrada_salida(char **io_ids, char **io_sleep) {
	lista_entrada_salida = list_create();
	int i = 0;
	while (io_ids[i] != NULL) {
		t_solicitudes_entrada_salida *io = malloc(
				sizeof(t_solicitudes_entrada_salida));
		io->nombre_dispositivo = malloc(strlen(io_ids[i]));
		io->nombre_dispositivo = io_ids[i];
		io->retardo = atoi(io_sleep[i]);
		io->solicitudes = queue_create();
		list_add(lista_entrada_salida, io);
		++i;
	}
}

//TODO poner en comunicaciones asi lo usan todos
void handshake(int socket, int proceso_receptor, int id_mensaje) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_NUCLEO;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = 0;

	if (enviar_header(socket, header) < sizeof(t_header)) {
		perror("Fallo al enviar handshake");
	}

	free(header);
}

void cargar_configuracion_nucleo(char *archivoConfig,
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

void pedir_pagina_tamanio(int socket_umc) {
	t_header* header_pido_tam_pag = malloc(sizeof(t_header));
	header_pido_tam_pag->id_proceso_emisor = 1;
	header_pido_tam_pag->id_proceso_receptor = 3;
	header_pido_tam_pag->id_mensaje = RECIBIR_TAMANIO_PAGINA;
	header_pido_tam_pag->longitud_mensaje = 0;
	enviar_header(socket_umc, header_pido_tam_pag);
	free(header_pido_tam_pag);
}
