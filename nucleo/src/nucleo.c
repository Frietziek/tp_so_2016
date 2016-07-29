/*
 ============================================================================
 Name        : nucleo.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//TODO logica de colas entrada cpu
//TODO semaforo que le diga a la cpu que hay un proceso en ready
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <serializacion.h>
#include <comunicaciones.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include "nucleo.h"
#include <commons/config.h>
#include <commons/string.h>
#include "serializacion_nucleo_consola.h"
#include "serializacion_nucleo_cpu.h"
#include "semaforos_nucleo.h"
#include "atiendo_cpu.h"

////////////////////VARIABLES GLOBALES///////////////////////////
//se llenan con t_pid
t_queue *cola_ready;
t_queue *cola_block;
t_queue *cola_exec;
t_queue *cola_new;
t_queue *cola_exit;
t_queue *cola_cpus; //se llena con t_cpu

t_dictionary *diccionario_entrada_salida;
int tamanio_pagina;
int socket_umc;
int pid_count;
t_list * lista_procesos; //t_fila_tabla_procesos*
t_config_nucleo *configuracion;
t_dictionary *variables_compartidas;

pthread_t thread_exec;

pthread_t thread_monitoreo_configuraciones; //En este thread se usa ionotify para monitorear el .ini y actualizar t_config_nucleo cuando sea necesario

int atiendo_solicitudes;

////////////////////FUNCION PRINCIPAL///////////////////////////

int main(void) {

	solicitudes_auxiliares_lista = list_create();

	logger_manager = log_create("nucleo.log", "NUCLEO", true, LOG_LEVEL_TRACE); // Creo archivo de log
	log_trace(logger_manager, "Proceso Nucleo creado.");
	pid_count = 1;
	atiendo_solicitudes = 1;

	//---------------------------------- Configuraciones ----------------------------------//
	configuracion = malloc(sizeof(t_config_nucleo));
	configuracion->ip_umc = malloc(30);
	cargar_configuracion_nucleo(PATH_CONFIGURACIONES_NUCLEO, configuracion);
	pthread_create(&thread_monitoreo_configuraciones, NULL,
			(void*) monitorear_configuraciones, NULL);
	//log_trace(logger_manager, "\nSe cargaron las configuraciones con los siguientes valores: \nIP_UMC=%s\nPUERTO_UMC=%i\nPUERTO_PROG=%i\nPUERTO_CPU=%i\nQUANTUM=%i\nQUANTUM_SLEEP=%i\nSEM_IDS=%s\nSEM_INIT=%s\nIO_IDS=%s\nIO_SLEEP=%s\nSHARED_VARS=%s\nSTACK_SIZE=%i\n", configuracion->ip_umc, configuracion->puerto_umc, configuracion->puerto_prog, configuracion->puerto_cpu, configuracion->quantum, configuracion->quantum_sleep, "es un array, no se logea por tiempo", "idem", "idem", "idem", "idem", configuracion->stack_size); //Descomentar para desarrollo, o hacer que imprima los array y dejar descomentado
	//-------------------------------------------------------------------------------------//

	lista_procesos = list_create();
	cola_ready = queue_create();
	cola_block = queue_create();
	cola_exec = queue_create();
	cola_new = queue_create();
	cola_exit = queue_create();
	cola_cpus = queue_create();

	sem_init(&mutex_pid_count, 0, 1);
	sem_init(&mutex_cola_ready, 0, 1);
	sem_init(&mutex_cola_exec, 0, 1);
	sem_init(&mutex_cola_block, 0, 1);
	sem_init(&mutex_solicitudes_auxiliares_lista, 0, 1);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&mutex_cola_exit, 0, 1);
	sem_init(&mutex_lista_procesos, 0, 1);
	sem_init(&mutex_variables_compartidas, 0, 1);
	sem_init(&mutex_diccionario_entrada_salida, 0, 1);
	sem_init(&mutex_solicitudes_semaforo, 0, 1);
	sem_init(&mutex_tabla_procesos, 0, 1);
	sem_init(&mutex_cola_cpu, 0, 1);
	sem_init(&mutex_cola_solicitudes, 0, 1);
	sem_init(&cant_cpu, 0, 0);
	sem_init(&cant_ready, 0, 0);
	sem_init(&cant_exec, 0, 0);
	sem_init(&cant_exit, 0, 0);
	sem_init(&cant_block, 0, 0);

	log_trace(logger_manager, "Inicializando colas entrada salida");
	inicializar_colas_entrada_salida(configuracion->io_id,
			configuracion->io_sleep);
	log_trace(logger_manager, "Inicializando variables compartidas");
	inicializar_variables_compartidas(configuracion->shared_vars);
	log_trace(logger_manager, "Inicializando semaforos");
	inicializar_solicitudes_semaforo(configuracion->sem_id,
			configuracion->sem_init);

	// INICIO PIDO LONGITUD PAGINA  AL UMC Y LO ATIENDO
	log_trace(logger_manager, " PIDO LONGITUD PAGINA  AL UMC");
	socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc, &atender_umc);
	pedir_pagina_tamanio(socket_umc);

	log_info(logger_manager, "Se cargo el tamanio de la pagina: %i",
			tamanio_pagina);

	log_trace(logger_manager, "Inicializando servidor consola");
	t_configuracion_servidor *configuracion_servidor_consola = malloc(
			sizeof(t_configuracion_servidor));
	configuracion_servidor_consola->puerto = configuracion->puerto_prog;
	configuracion_servidor_consola->funcion = &atender_consola;

	crear_servidor(configuracion_servidor_consola);
	log_trace(logger_manager, "Servidor consola creado");
	//FIN ATIENDO CONSOLA

	//INICIO ATIENDO CPU
	log_trace(logger_manager, "Inicializando servidor cpu");
	t_configuracion_servidor *configuracion_servidor_cpu = malloc(
			sizeof(t_configuracion_servidor));
	configuracion_servidor_cpu->puerto = configuracion->puerto_cpu;
	configuracion_servidor_cpu->funcion = &atender_cpu;
	configuracion_servidor_cpu->parametros_funcion = configuracion;

	crear_servidor(configuracion_servidor_cpu);
	log_trace(logger_manager, "Servidor cpu creado");
	//FIN ATIENDO CPU

	pthread_create(&thread_exec, NULL, (void*) asignar_pcb_a_cola_exec, NULL);

	getchar();

	log_trace(logger_manager, "Cerrando Nucleo");
	//Libero antes de cerrar
	free(configuracion->ip_umc);
	free(configuracion);
	free(configuracion_servidor_cpu);
	free(configuracion_servidor_consola);

	//todo liberar todas las estructuras que faltan
	queue_destroy(cola_block);
	queue_destroy(cola_ready);
	queue_destroy(cola_exec);
	queue_destroy(cola_cpus);

	sem_destroy(&mutex_pid_count);
	sem_destroy(&mutex_cola_block);
	sem_destroy(&mutex_cola_ready);
	sem_destroy(&mutex_cola_exec);
	sem_destroy(&mutex_cola_cpu);
	sem_destroy(&cant_cpu);
	sem_destroy(&cant_ready);
	sem_destroy(&cant_exec);
	sem_destroy(&cant_exit);
	sem_destroy(&cant_block);

	log_trace(logger_manager, "Se cerro nucleo");
	return EXIT_SUCCESS;
}

////////////////////FUNCIONES ATENDER///////////////////////////

void atender_umc(t_paquete *paquete, int socket_conexion) {

	t_pid *pid_a_matar;

	switch (paquete->header->id_mensaje) {
	case REPUESTA_HANDSHAKE_UMC:
		;
		t_pagina_tamanio *pagina = malloc(sizeof(t_pagina_tamanio));
		deserializar_pagina_tamanio(paquete->payload, pagina);
		tamanio_pagina = pagina->tamanio;
		log_info(logger_manager, "Se cargo el tamanio de la pagina: %i",
				tamanio_pagina);
		free(pagina);
		break;
	case RESPUESTA_INICIALIZAR_PROGRAMA: //recibo un t_pid con el pid del proceso a poner en cola ready
		;
		t_pid *pid_a_ready = malloc(sizeof(t_pid));
		deserializar_pid(paquete->payload, pid_a_ready);
		log_info(logger_manager,
				"Recibi confirmacion de proceso creado en memoria con pid: %d",
				pid_a_ready->pid);

		sem_wait(&mutex_cola_new);
		t_pcb *pcb_a_ready = queue_pop_pid(cola_new, pid_a_ready->pid);
		sem_post(&mutex_cola_new);

		sem_wait(&mutex_cola_ready);
		queue_push(cola_ready, pcb_a_ready);
		log_info(logger_manager, "PID %d - Se agrega a la cola READY",
				pcb_a_ready->pid);
		sem_post(&mutex_cola_ready);

		sem_wait(&mutex_tabla_procesos);
		pcb_a_ready->estado = READY;
		sem_post(&mutex_tabla_procesos);

		sem_post(&cant_ready);

		break;
	case RESPUESTA_FINALIZAR_PROGRAMA:
		;
		pid_a_matar = malloc(sizeof(t_pid));
		deserializar_pid(paquete->payload, pid_a_matar);
		log_info(logger_manager, "Finalizo en UMC el pcb creado con pid: %d",
				pid_a_matar->pid);
		sem_wait(&mutex_cola_exit);
		t_pcb * pcb_a_matar = queue_pop_pid(cola_exit, pid_a_matar->pid);
		sem_post(&mutex_cola_exit);
		finalizar_proceso_en_lista_proc_con_socket_cpu(pcb_a_matar,
				socket_conexion);
		free(pid_a_matar);
		libero_pcb(pcb_a_matar);
		break;
	case RESPUESTA_MATAR_PROGRAMA: //recibo un t_pid con el pid del proceso a eliminar
		;
		pid_a_matar = malloc(sizeof(t_pid));
		deserializar_pid(paquete->payload, pid_a_matar);
		log_info(logger_manager, "Elimino de UMC el pcb creado con pid: %d",
				pid_a_matar->pid);
		sem_wait(&mutex_cola_exit);
		pcb_a_matar = queue_pop_pid(cola_exit, pid_a_matar->pid);
		sem_post(&mutex_cola_exit);
		finalizar_proceso_en_lista_proc_con_socket_cpu(pcb_a_matar,
				socket_conexion);
		free(pid_a_matar);
		libero_pcb(pcb_a_matar);
		break;
	case ERROR_INICIALIZAR_PROGRAMA: //recibo un t_pid con el pid del proceso a eliminar
		;
		t_pid *pid_a_eliminar = malloc(sizeof(t_pid));
		deserializar_pid(paquete->payload, pid_a_eliminar);
		log_info(logger_manager,
				"Error al inicializar programa. Elimino el pcb creado con pid: %d",
				pid_a_eliminar->pid);
		int socket_consola = buscar_socket_consola_por_pid(pid_a_eliminar->pid);
		sem_wait(&mutex_cola_new);
		pcb_a_matar = queue_pop_pid(cola_new, pid_a_eliminar->pid); //remuevo el puntero al pcb de la cola new
		sem_post(&mutex_cola_new);
		eliminar_proceso_de_lista_procesos_con_pid(pid_a_eliminar->pid);
		enviar_header_completado(socket_consola, PROCESO_CONSOLA,
		MENSAJE_ERROR_AL_INICIAR);
		free(pid_a_eliminar);
		libero_pcb(pcb_a_matar);
		break;
	case ERROR_MATAR_PROGRAMA: //Se presume que si da error deberia estar en la cola new.
		;
		t_pid *pid_ = malloc(sizeof(t_pid));
		deserializar_pid(paquete->payload, pid_);
		t_pcb *pcb_a_kill = buscar_pcb_por_pid(pid_->pid);
		int socket_conso = buscar_socket_consola_por_pid(pid_->pid);
		sem_wait(&mutex_cola_new);
		queue_pop_pid(cola_new, pcb_a_kill->pid); //remuevo el puntero al pcb de la cola new
		sem_post(&mutex_cola_new);
		log_info(logger_manager, "Elimino el pcb creado con pid: %d",
				pid_->pid);
		eliminar_proceso_de_lista_procesos_con_pid(pid_->pid);
		enviar_header_completado(socket_conso, PROCESO_CONSOLA,
		MENSAJE_ERROR_AL_MATAR);
		free(pid_);
		break;
	default:
		log_warning(logger_manager, "Mensaje no reconocido de la UMC: %i",
				paquete->header->id_mensaje);
		break;
	}
}

void atender_consola(t_paquete *paquete_buffer, int socket_consola) {

	switch (paquete_buffer->header->id_mensaje) {
	case INICIAR:
		;
		//inicio para ver lo que contiene el payload,
		t_texto *codigo_de_consola = malloc(sizeof(t_texto));

		deserializar_codigo(paquete_buffer->payload, codigo_de_consola);
		printf("el codigo es:\n %s\n\n", codigo_de_consola->texto);

		//fin para ver lo que contiene el payload,

		t_pcb *pcb = crear_PCB(codigo_de_consola->texto);

		sem_wait(&mutex_cola_new);
		queue_push(cola_new, pcb);
		log_info(logger_manager, "PID %d - Se agrega a la cola NEW", pcb->pid);
		sem_post(&mutex_cola_new);

		agregar_pcb_a_lista_procesos(pcb, socket_consola);

		enviar_programa_completo_a_umc(pcb->pid, pcb->cant_paginas_codigo_stack,
				codigo_de_consola->texto);
		free(codigo_de_consola->texto);
		free(codigo_de_consola);
		break;
	case MATAR:

		sem_wait(&mutex_lista_procesos);
		t_pcb *pcb_obtenido = buscar_pcb_por_socket_consola(socket_consola);
		int pid = pcb_obtenido->pid;
		log_info(logger_manager, "Se va a terminar la ejecucion del pid: %d",
				pid);
		matar_ejecucion(pcb_obtenido);
		log_info(logger_manager, "Se termino la ejecucion del pid: %d", pid);
		eliminar_proceso_de_lista_procesos_con_pid(pid);
		sem_post(&mutex_lista_procesos);
		enviar_header_completado(socket_consola, PROCESO_CONSOLA,
		MENSAJE_MATAR_OK);

		break;
	case HANDSHAKE_CONSOLA:
		if (paquete_buffer->header->id_proceso_emisor == PROCESO_CONSOLA) {

			log_trace(logger_manager, "Se recibio el handshake de la consola");

			t_header *header = malloc(sizeof(t_header));
			header->id_proceso_emisor = PROCESO_NUCLEO;
			header->id_proceso_receptor = PROCESO_CONSOLA;

			header->id_mensaje = MENSAJE_HANDSHAKE_RECIBIDO_CONSOLA;
			header->longitud_mensaje = 0;

			int cantidad_bytes_enviados = enviar_header(socket_consola, header);

			if (cantidad_bytes_enviados < sizeof(t_header))
				log_error(logger_manager,
						"Ocurrio un problema al enviar la respuesta del handshake a la consola");
			else
				log_trace(logger_manager,
						"Se realizó el envio de la respuesta del handshake de la consola correctamente");

			free(header);

		} else {
			log_trace(logger_manager,
					"Se recibio el handshake de un proceso no esperado");
		}

		break;
	case RESPUESTA_IMPRIMIR:
		log_info(logger_manager,
				"La consola imprimió el valor satisfactoriamente");
		break;
	case RESPUESTA_IMPRIMIR_TEXTO:
		log_info(logger_manager,
				"La consola imprimió el texto satisfactoriamente");
		break;
	case RESPUESTA_PROGRAMA_FINALIZADO_CONSOLA:
		log_info(logger_manager,
				"El programa de la consola: %i finalizo correctamente",
				paquete_buffer->header->id_proceso_emisor);
		break;
		//todo faltan varios mensajes de error de consola,
	default:
		log_warning(logger_manager, "Mensaje no reconocido de la Consola: %i",
				paquete_buffer->header->id_mensaje);
		break;
	}

}

void respuesta_matar(void * buffer, int socket_cpu) {
	t_pcb_quantum *pcb_quantum = malloc(sizeof(t_pcb_quantum));
	deserializar_pcb_quantum(buffer, pcb_quantum);

	sem_wait(&mutex_cola_exec);
	t_pcb * pcb_a_matar = queue_pop_pid(cola_exec, pcb_quantum->pcb->pid);
	sem_post(&mutex_cola_exec);

	agregar_cpu_disponible(socket_cpu);
	sem_post(&cant_cpu);

	libero_pcb(pcb_a_matar);
}

////////////////////FUNCIONES AUXILIARES///////////////////////////

void eliminar_proceso_de_lista_procesos_con_pid(int pid) {
	sem_wait(&mutex_lista_procesos);

	bool eliminar_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pid = proceso->pcb->pid);

	}

	void process_info_destroy(t_fila_tabla_procesos *proceso) {
		libero_pcb(proceso->pcb);
		free(proceso);
	}

	list_remove_and_destroy_by_condition(lista_procesos,
			(void*) eliminar_proceso_logica, (void*) process_info_destroy);
	sem_post(&mutex_lista_procesos);

}
t_pcb *buscar_pcb_por_socket_consola(int socket_consola) {

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (socket_consola == proceso->socket_consola);
	}
	t_pcb *pcb = (((t_fila_tabla_procesos*) list_find(lista_procesos,
			(void*) busqueda_proceso_logica)))->pcb;

	return pcb;
}

t_pcb *buscar_pcb_por_pid(int pid) {

	sem_wait(&mutex_lista_procesos);

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pid == proceso->pcb->pid);
	}
	t_pcb *pcb_encontrado = (((t_fila_tabla_procesos*) list_find(lista_procesos,
			(void*) busqueda_proceso_logica)))->pcb;
	sem_post(&mutex_lista_procesos);
	return pcb_encontrado;
}

int buscar_socket_consola_por_socket_cpu(int socket_cpu) {

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (socket_cpu == proceso->socket_cpu);
	}
	int socket_consola_encontrado = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)))->socket_consola;

	return socket_consola_encontrado;
}

void cambiar_estado_proceso_por_pid(int pid, int estado) {
	sem_wait(&mutex_lista_procesos);
	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pid == proceso->pcb->pid);
	}
	t_pcb *pcb = (((t_fila_tabla_procesos*) list_find(lista_procesos,
			(void*) busqueda_proceso_logica)))->pcb;
	pcb->estado = estado;
	sem_post(&mutex_lista_procesos);
}

int buscar_socket_consola_por_pid(int pid) {
	sem_wait(&mutex_lista_procesos);

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pid == proceso->pcb->pid);
	}
	int socket_consola_encontrado = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)))->socket_consola;
	sem_post(&mutex_lista_procesos);
	return socket_consola_encontrado;
}

t_pcb *buscar_pcb_por_socket_cpu(int socket_cpu) {
	sem_wait(&mutex_lista_procesos);

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (socket_cpu == proceso->socket_cpu);
	}
	t_fila_tabla_procesos *fila = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)));
	if (fila != NULL) {
		t_pcb *pcb = fila->pcb;
		sem_post(&mutex_lista_procesos);
		return pcb;
	}
	sem_post(&mutex_lista_procesos);
	return NULL;
}

void enviar_error_al_iniciar_a_consola(int socket_consola) {

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
	free(payload_programa_completo->contenido_buffer);
	free(payload_programa_completo);

}

void agregar_pcb_a_lista_procesos(t_pcb *pcb, int socket_consola) {
	t_fila_tabla_procesos *fila = malloc(sizeof(t_fila_tabla_procesos));
	fila->pcb = pcb;
	fila->socket_consola = socket_consola;
	fila->socket_cpu = NO_ASIGNADO;
	sem_wait(&mutex_lista_procesos);
	list_add(lista_procesos, fila);
	sem_post(&mutex_lista_procesos);
}

int obtener_cantidad_paginas_codigo_stack(char *codigo_de_consola) {
	int modulo = strlen(codigo_de_consola) % tamanio_pagina;
	int division = strlen(codigo_de_consola) / tamanio_pagina;
	if (modulo == 0) {
		return division + configuracion->stack_size;
	} else
		return division + 1 + configuracion->stack_size;
}

t_pcb *crear_PCB(char *codigo_de_consola) {

	t_metadata_program *metadata = malloc(sizeof(t_metadata_program));
	metadata = metadata_desde_literal(codigo_de_consola);

	t_pcb *pcb = malloc(sizeof(t_pcb));
	sem_wait(&mutex_pid_count);
	pcb->pid = pid_count;
	++pid_count;
	sem_post(&mutex_pid_count);
	pcb->pc = metadata->instruccion_inicio;
	pcb->cant_paginas_codigo_stack = obtener_cantidad_paginas_codigo_stack(
			codigo_de_consola);
	pcb->estado = NEW;
	pcb->contexto_actual = 0;
	pcb->stack_position = strlen(codigo_de_consola);
	pcb->stack_pointer = pcb->stack_position;
	pcb->etiquetas_size = metadata->etiquetas_size;
	pcb->etiquetas = metadata->etiquetas;
	pcb->instrucciones_size = metadata->instrucciones_size;
	pcb->instrucciones_serializadas = metadata->instrucciones_serializado;
	pcb->stack_size_fisico = configuracion->stack_size;
	pcb->stack_size = 1;
	pcb->indice_stack = malloc(sizeof(t_indice_stack));
	pcb->indice_stack->cantidad_variables = 0;
	pcb->indice_stack->posicion_retorno = 0;
	pcb->indice_stack->posicion_variable_retorno = malloc(
			sizeof(t_posicion_memoria));
	pcb->indice_stack->posicion_variable_retorno->pagina = 0;
	pcb->indice_stack->posicion_variable_retorno->offset = 0;
	pcb->indice_stack->posicion_variable_retorno->size = 0;

	return pcb;
}

void matar_ejecucion(t_pcb *pcb_a_finalizar) {

	if (pcb_a_finalizar->estado != EXIT) {

		t_pcb * pcb_a_matar;

		t_pid *finalizar = malloc(sizeof(t_pid));

		finalizar->pid = pcb_a_finalizar->pid;

		t_buffer *buffer_finalizar = serializar_pid(finalizar);

		// ENVIO TERMINAR AL CPU

		if (pcb_a_finalizar->estado == EXEC) {

			int socket_cpu = buscar_socket_cpu_por_pcb(pcb_a_finalizar);

			enviar_header_completado(socket_cpu, PROCESO_CPU, MENSAJE_MATAR);
		}

		if (pcb_a_finalizar->estado == READY) {
			sem_wait(&mutex_cola_ready);
			pcb_a_matar = queue_pop_pid(cola_ready, pcb_a_finalizar->pid);
			sem_post(&mutex_cola_ready);
			//sem_wait(&cant_ready);
			libero_pcb(pcb_a_matar);
		}
		if (pcb_a_finalizar->estado == NEW) {
			sem_wait(&mutex_cola_new);
			pcb_a_matar = queue_pop_pid(cola_new, pcb_a_finalizar->pid);
			sem_post(&mutex_cola_new);
			libero_pcb(pcb_a_matar);
		}
		if (pcb_a_finalizar->estado == BLOCK) {
			sem_wait(&mutex_cola_block);
			pcb_a_matar = queue_pop_pid(cola_block, pcb_a_finalizar->pid);
			sem_post(&mutex_cola_block);
			libero_pcb(pcb_a_matar);
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

	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pcb_a_finalizar->pid == proceso->pcb->pid);
	}
	int socket_cpu = (((t_fila_tabla_procesos*) list_find(lista_procesos,
			(void*) busqueda_proceso_logica)))->socket_cpu;

	return socket_cpu;
}

void inicializar_variables_compartidas(char **shared_vars) {
	sem_wait(&mutex_variables_compartidas);
	variables_compartidas = dictionary_create();
	int i = 0;
	while (shared_vars[i] != NULL) {
		t_valor_variable_compartida *valor_variable_compartida = malloc(
				sizeof(t_valor_variable_compartida));
		valor_variable_compartida->valor =
		VALOR_INICIAL_VARIABLE_COMPARTIDA;
		dictionary_put(variables_compartidas, shared_vars[i],
				valor_variable_compartida);
		++i;
	}
	sem_post(&mutex_variables_compartidas);
}

void inicializar_solicitudes_semaforo(char **sem_id, char**sem_ini) {
	int i = 0;
	solitudes_semaforo = dictionary_create();

	while (sem_id[i] != NULL || sem_ini[i] != NULL) {
		++i;
	}
	pthread_t desbloq_pcb_semaforo[i];
	i = 0;
	while (sem_id[i] != NULL || sem_ini[i] != NULL) {
		t_atributos_semaforo *atributos = malloc(sizeof(t_atributos_semaforo));
		atributos->valor = atoi(sem_ini[i]);
		atributos->posicion_semaforo_contador_solicitudes = i;
		sem_init(&sem_semaforos[i], 0, 0);
//		sem_init(&(sem_semaforos[i]), 0, 0);
		atributos->solicitudes = queue_create();
		sem_wait(&mutex_solicitudes_semaforo);
		dictionary_put(solitudes_semaforo, sem_id[i], atributos);
		sem_post(&mutex_solicitudes_semaforo);
		pthread_create(&desbloq_pcb_semaforo[i], NULL,
				(void*) desbloquear_pcb_semaforo, atributos);
		++i;

	}
}

void inicializar_colas_entrada_salida(char **io_ids, char **io_sleep) {
	diccionario_entrada_salida = dictionary_create();
	int i = 0;
	while (io_ids[i] != NULL) {
		++i;
	}
	pthread_t hilo_io[i];
	i = 0;
	while (io_ids[i] != NULL) {
		t_solicitudes_entrada_salida *io = malloc(
				sizeof(t_solicitudes_entrada_salida));
		io->retardo = atoi(io_sleep[i]);
		io->posicion_array_semaforo = i;
		sem_init(&sem_dispositivo[i], 0, 0);
		io->solicitudes = queue_create();
		dictionary_put(diccionario_entrada_salida, io_ids[i], io);
		pthread_create(&hilo_io[i], NULL,
				(void*) atender_solicitudes_entrada_salida, io);
		++i;
	}
}

void desbloquear_pcb_semaforo(t_atributos_semaforo *atributos) {
	while (atiendo_solicitudes) {
		sem_wait(
				&(sem_semaforos[atributos->posicion_semaforo_contador_solicitudes])); //avanzo si hay un proceso en la cola de solicitudes

		if (queue_size(atributos->solicitudes) > 0) {

			sem_wait(&mutex_cola_solicitudes);
			t_pid *pid = queue_pop(atributos->solicitudes);
			sem_post(&mutex_cola_solicitudes);

			//t_pcb *pcb_a_ready = buscar_pcb_por_pid(pid->pid);

			//decrementar_semaforo(configuracion->sem_id[atributos->posicion_semaforo_contador_solicitudes]);

			sem_wait(&mutex_cola_block);
			t_pcb * pcb_a_ready = queue_pop_pid(cola_block, pid->pid);
			sem_post(&mutex_cola_block);

			actualizar_estado_pcb_y_saco_socket_cpu(pcb_a_ready, READY);

			sem_wait(&mutex_cola_ready);
			queue_push(cola_ready, pcb_a_ready);
			log_info(logger_manager, "PID %d - Se agrega a la cola READY",
					pcb_a_ready->pid);
			sem_post(&mutex_cola_ready);

			sem_post(&cant_ready);
		} else {
			aumentar_semaforo(
					configuracion->sem_id[atributos->posicion_semaforo_contador_solicitudes]);
		}
		sem_post(&mutex_solicitudes_semaforo);
	}

}

void atender_solicitudes_entrada_salida(t_solicitudes_entrada_salida *io) {
	while (atiendo_solicitudes) {

		sem_wait(&(sem_dispositivo[io->posicion_array_semaforo])); //avanzo si hay un proceso en la cola de solicitudes
		t_solicitud_entrada_salida_cpu * solicitud = queue_pop(io->solicitudes);

		log_info(logger_manager,
				"Comienza io del socket cpu :%d ,con retardo de: %d",
				solicitud->socket_cpu, io->retardo);
		//10* porque paso de milisegundo a microsegundo
		usleep(10 * io->retardo * solicitud->cantidad_operaciones);

		//sem_post(&(sem_dispositivo[io->posicion_array_semaforo]));
		log_info(logger_manager,
				"Termina io del socket cpu :%d ,con retardo de: %d",
				solicitud->socket_cpu, io->retardo);

		free(solicitud);

		sem_wait(&mutex_cola_block);
		t_pcb * pcb_a_ready = queue_pop_pid(cola_block, solicitud->pid);
		sem_post(&mutex_cola_block);

		sem_wait(&mutex_cola_ready);
		queue_push(cola_ready, pcb_a_ready);
		actualizar_estado_pcb_y_saco_socket_cpu(pcb_a_ready, READY);
		sem_post(&mutex_cola_ready);
		log_info(logger_manager, "PID %d - Se agrega a la cola READY",
				pcb_a_ready->pid);
		sem_post(&cant_ready);
	}
}

void enviar_header_completado(int socket, int proceso_receptor, int id_mensaje) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_NUCLEO;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = 0;

	if (enviar_header(socket, header) < sizeof(t_header)) {
		perror("Fallo al enviar enviar_header_completado");
	}

	free(header);
}

void cargar_configuracion_nucleo(char *archivoConfig,
		t_config_nucleo *configuracion_nucleo) {

	t_config *configuracion = config_create(archivoConfig);

	if (config_has_property(configuracion, "PUERTO_PROG")) {
		configuracion_nucleo->puerto_prog = config_get_int_value(configuracion,
				"PUERTO_PROG");
	} else {
		perror("error al cargar PUERTO_PROG");
	}
	if (config_has_property(configuracion, "PUERTO_CPU")) {
		configuracion_nucleo->puerto_cpu = config_get_int_value(configuracion,
				"PUERTO_CPU");
	} else {
		perror("error al cargar PUERTO_CPU");
	}
	if (config_has_property(configuracion, "QUANTUM")) {
		configuracion_nucleo->quantum = config_get_int_value(configuracion,
				"QUANTUM");
	} else {
		perror("error al cargar QUANTUM");
	}
	if (config_has_property(configuracion, "QUANTUM_SLEEP")) {
		configuracion_nucleo->quantum_sleep = config_get_int_value(
				configuracion, "QUANTUM_SLEEP");
	} else {
		perror("error al cargar QUANTUM_SLEEP");
	}

	if (config_has_property(configuracion, "IO_IDS")) {
		configuracion_nucleo->io_id = config_get_array_value(configuracion,
				"IO_IDS");
	} else {
		perror("error al cargar IO_IDS");
	}
	if (config_has_property(configuracion, "IO_SLEEP")) {
		configuracion_nucleo->io_sleep = config_get_array_value(configuracion,
				"IO_SLEEP");
	} else {
		perror("error al cargar IO_SLEEP");
	}

	if (config_has_property(configuracion, "SEM_IDS")) {
		configuracion_nucleo->sem_id = config_get_array_value(configuracion,
				"SEM_IDS");
	} else {
		perror("error al cargar SEM_IDS");
	}

	if (config_has_property(configuracion, "SEM_INIT")) {
		configuracion_nucleo->sem_init = config_get_array_value(configuracion,
				"SEM_INIT");
	} else {
		perror("error al cargar SEM_INIT");
	}

	if (config_has_property(configuracion, "SHARED_VARS")) {
		configuracion_nucleo->shared_vars = config_get_array_value(
				configuracion, "SHARED_VARS");
	} else {
		perror("error al cargar SHARED_VARS");
	}

	if (config_has_property(configuracion, "IP_UMC")) {
		strcpy(configuracion_nucleo->ip_umc,
				config_get_string_value(configuracion, "IP_UMC")); //Hago la copia para así despues usar config_destroy y que no explote todo
		//configuracion_nucleo->ip_umc = config_get_string_value(configuracion,"IP_UMC");
	} else {
		perror("error al cargar IP_UMC");
	}

	if (config_has_property(configuracion, "PUERTO_UMC")) {
		configuracion_nucleo->puerto_umc = config_get_int_value(configuracion,
				"PUERTO_UMC");
	} else {
		perror("error al cargar PUERTO_UMC");
	}
	if (config_has_property(configuracion, "STACK_SIZE")) {
		configuracion_nucleo->stack_size = config_get_int_value(configuracion,
				"STACK_SIZE");
	} else {
		perror("error al cargar STACK_SIZE");
	}

//free(configuracion);
	config_destroy(configuracion);
}

void pedir_pagina_tamanio(int socket_umc) {
	t_header* header_pido_tam_pag = malloc(sizeof(t_header));
	header_pido_tam_pag->id_proceso_emisor = 1;
	header_pido_tam_pag->id_proceso_receptor = 3;
	header_pido_tam_pag->id_mensaje = MENSAJE_HANDSHAKE_UMC;
	header_pido_tam_pag->longitud_mensaje = 0;
	enviar_header(socket_umc, header_pido_tam_pag);
	free(header_pido_tam_pag);
}

int obtener_variable_compartida(char *nombre_variable_compartida) {
	sem_wait(&mutex_variables_compartidas);
	t_valor_variable_compartida *valor_variable_compartida = dictionary_get(
			variables_compartidas, nombre_variable_compartida);
	sem_post(&mutex_variables_compartidas);
	return valor_variable_compartida->valor;
}

void asignar_variable_compartida(char *nombre_variable_compartida, int valor) {
	sem_wait(&mutex_variables_compartidas);
	t_valor_variable_compartida *var_comp = dictionary_get(
			variables_compartidas, nombre_variable_compartida);
	var_comp->valor = valor;
	sem_post(&mutex_variables_compartidas);
}

void bloquear_pcb_dispositivo(int socket_cpu, t_entrada_salida * entrada_salida) {
	t_solicitud_entrada_salida_cpu *solicitud_io = malloc(
			sizeof(t_solicitud_entrada_salida_cpu));
	solicitud_io->cantidad_operaciones = entrada_salida->tiempo;
	solicitud_io->socket_cpu = socket_cpu;
	solicitud_io->pid = entrada_salida->pid;
	sem_wait(&mutex_diccionario_entrada_salida);

	t_solicitudes_entrada_salida *solicitudes_es = dictionary_get(
			diccionario_entrada_salida, entrada_salida->nombre_dispositivo);

	queue_push(solicitudes_es->solicitudes, solicitud_io);
	sem_post(&mutex_diccionario_entrada_salida);

	sem_post(&(sem_dispositivo[solicitudes_es->posicion_array_semaforo]));

	free(entrada_salida);
}

/*void bloquear_pcb_semaforo(char *nombre_semaforo, int socket_cpu) {
 t_valor_socket_cola_semaforos *socket = malloc(
 sizeof(t_valor_socket_cola_semaforos));
 socket->socket = socket_cpu;
 t_pcb *pcb = buscar_pcb_por_socket_cpu(socket_cpu);

 sem_wait(&mutex_solicitudes_semaforo);

 t_pid *pid = malloc(sizeof(t_pid));
 pid->pid = pcb->pid;
 //se bloquea el proceso, la cpu no se bloquea
 queue_push(
 ((t_atributos_semaforo*) dictionary_get(solitudes_semaforo,
 nombre_semaforo))->solicitudes, pid);
 sem_post(&mutex_solicitudes_semaforo);

 sacar_socket_cpu_de_tabla(socket_cpu);

 sem_wait(&mutex_cola_exec);
 bool busqueda_pcb(t_pcb *_pcb) {
 return (pcb->pid == _pcb->pid);
 }
 list_remove_by_condition(cola_exec->elements, (void *) busqueda_pcb);
 sem_post(&mutex_cola_exec);

 sem_wait(&mutex_cola_block);
 queue_push(cola_block, pcb);
 log_info(logger_manager, "PROCESO %d - Se agrega a la cola BLOCK",
 pcb->pid);
 sem_post(&mutex_cola_block);

 free(socket);
 }*/

void sacar_socket_cpu_de_tabla(int socket_cpu) {

	sem_wait(&mutex_lista_procesos);
	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (socket_cpu == proceso->socket_cpu);
	}
	t_fila_tabla_procesos *proceso = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)));
	if (proceso != NULL) { //todo revisar por que no lo esta sacando aca
		proceso->socket_cpu = NO_ASIGNADO;
	}
	sem_post(&mutex_lista_procesos);

}

void envio_buffer_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio, t_buffer *buffer) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = buffer->longitud_buffer;

	if (enviar_buffer(socket_proceso, header, buffer)
			< sizeof(t_header) + buffer->longitud_buffer) {
		log_error(logger_manager, mensaje_fallo_envio);
	}

	free(header);
}

void asignar_pcb_a_cola_exec() {
	while (1) {
		int socket;
		sem_wait(&cant_ready);		//avanzo si hay un proceso en la cola ready

		sem_wait(&cant_cpu);		// y si hay una cpu libre

		sem_wait(&mutex_cola_cpu);
		t_cpu * cpu = queue_pop(cola_cpus);
		socket = cpu->socket_cpu;
		sem_post(&mutex_cola_cpu);

		asignar_pcb_a_cpu(socket);
		free(cpu);
	}
}

void asignar_pcb_a_cpu(int socket_cpu) {
	t_pcb_quantum *pcb_quantum_a_cpu = malloc(sizeof(t_pcb_quantum));
	t_pcb * pcb_a_exec;

	sem_wait(&mutex_cola_ready);
	pcb_a_exec = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);

	sem_wait(&mutex_cola_exec);
	queue_push(cola_exec, pcb_a_exec);
	log_info(logger_manager, "PID %i - Se agrega a la cola EXEC",
			pcb_a_exec->pid);
	sem_post(&mutex_cola_exec);

	actualizar_pcb_y_ponerlo_en_exec_con_socket_cpu(pcb_a_exec, socket_cpu);
	pcb_quantum_a_cpu->pcb = pcb_a_exec;

	pcb_quantum_a_cpu->quantum = configuracion->quantum;
	t_buffer *pcb_quantum_buffer = serializar_pcb_quantum(pcb_quantum_a_cpu);
	envio_buffer_a_proceso(socket_cpu, PROCESO_CPU, MENSAJE_PCB_NUCLEO,
			"error al enviar pcb quantum a cpu", pcb_quantum_buffer);
	log_info(logger_manager, "Se envio el Proceso: %i a la CPU: %i",
			pcb_a_exec->pid, socket_cpu);
	free(pcb_quantum_a_cpu);
	free(pcb_quantum_buffer);
}

int wait_semaforo(char *semaforo_nombre) {
	sem_wait(&mutex_solicitudes_semaforo);
	(((t_atributos_semaforo*) dictionary_get(solitudes_semaforo,
			semaforo_nombre))->valor)--;
	int valor_semaforo = (((t_atributos_semaforo*) dictionary_get(
			solitudes_semaforo, semaforo_nombre))->valor);
	sem_post(&mutex_solicitudes_semaforo);
	return valor_semaforo;
}

int obtener_valor_semaforo(char *semaforo_nombre) {

	//sem_wait(&mutex_solicitudes_semaforo);
	int valor_semaforo = (((t_atributos_semaforo*) dictionary_get(
			solitudes_semaforo, semaforo_nombre))->valor);
	//sem_post(&mutex_solicitudes_semaforo);

	return valor_semaforo;
}

/*int signal_semaforo(char *semaforo_nombre) {

 aumentar_semaforo(semaforo_nombre);
 (((t_atributos_semaforo*) dictionary_get(solitudes_semaforo,
 semaforo_nombre))->valor)++;
 int valor_semaforo = (((t_atributos_semaforo*) dictionary_get(
 solitudes_semaforo, semaforo_nombre))->valor);

 return valor_semaforo;
 }*/

void avisar_para_que_desbloquee(char *nombre_sem) {

	t_atributos_semaforo* atributos = dictionary_get(solitudes_semaforo,
			nombre_sem);
	sem_post(
			&(sem_semaforos[atributos->posicion_semaforo_contador_solicitudes]));
}

void atiendo_quantum(void *buffer, int socket_conexion) {
// Recibo PCB
	t_pcb_quantum *pcb_quantum = malloc(sizeof(t_pcb_quantum));
	deserializar_pcb_quantum(buffer, pcb_quantum);

	sem_wait(&mutex_cola_exec);
	t_pcb * pcb_out = queue_pop_pid(cola_exec, pcb_quantum->pcb->pid);
	sem_post(&mutex_cola_exec);

	sem_wait(&mutex_cola_ready);
	queue_push(cola_ready, pcb_quantum->pcb);
	log_info(logger_manager, "PID %d - Se agrega a la cola READY",
			pcb_quantum->pcb->pid);
	agregar_cpu_disponible(socket_conexion);
	actualizar_pcb_y_ponerlo_en_ready_con_socket_cpu(pcb_quantum->pcb,
			socket_conexion);

	log_info(logger_manager, "Agregue pid: %d a cola ready",
			pcb_quantum->pcb->pid);
	libero_pcb(pcb_out);

	sem_post(&mutex_cola_ready);
	sem_post(&cant_ready);

}

void libero_pcb(t_pcb *pcb) {
	/*t_indice_stack* indice_stack = pcb->indice_stack;
	 int i_stack;
	 for (i_stack = 0; i_stack < pcb->stack_size; ++i_stack) {
	 indice_stack += i_stack;
	 int i_variables;
	 for (i_variables = 0; i_variables < indice_stack->cantidad_variables;
	 ++i_variables) {
	 t_variables_stack* indice_variables = indice_stack->variables;
	 indice_variables += i_variables;
	 free(indice_variables->posicion_memoria);
	 }
	 free(indice_stack->posicion_variable_retorno);
	 //	free(indice_stack->variables);
	 }*/
	free(pcb->instrucciones_serializadas);
	free(pcb->indice_stack);
	free(pcb->etiquetas);
	free(pcb);
}

void atiendo_programa_finalizado(void *buffer, int socket_cpu) {
// Recibo PCB
	t_pcb_quantum *pcb_quantum = malloc(sizeof(t_pcb_quantum));
	deserializar_pcb_quantum(buffer, pcb_quantum);
//finalizar_proceso_en_lista_proc_con_socket_cpu(pcb_quantum->pcb,socket_cpu);

	sem_wait(&mutex_cola_exec);
	t_pcb * pcb_out = queue_pop_pid(cola_exec, pcb_quantum->pcb->pid);
	sem_post(&mutex_cola_exec);

	agregar_cpu_disponible(socket_cpu);

	sem_wait(&mutex_cola_exit);
	queue_push(cola_exit, pcb_quantum->pcb);
	log_info(logger_manager, "PID %d - Se agrega a la cola EXIT", pcb_out->pid);
	actualizar_estado_pcb_y_saco_socket_cpu(pcb_quantum->pcb, EXIT);
	sem_post(&mutex_cola_exit);
//sem_post(&cant_exit);

// ENVIO TERMINAR AL UMC

	t_pid *finalizar = malloc(sizeof(t_pid));

	finalizar->pid = pcb_quantum->pcb->pid;

	t_buffer *buffer_finalizar = serializar_pid(finalizar);

	t_header *header_finalizar_umc = malloc(sizeof(t_header));

	header_finalizar_umc->id_proceso_emisor = PROCESO_NUCLEO;
	header_finalizar_umc->id_proceso_receptor = PROCESO_UMC;
	header_finalizar_umc->id_mensaje = MENSAJE_FINALIZAR_PROGRAMA;
	header_finalizar_umc->longitud_mensaje = buffer_finalizar->longitud_buffer;

	if (enviar_buffer(socket_umc, header_finalizar_umc, buffer_finalizar)
			< sizeof(t_header) + buffer_finalizar->longitud_buffer) {
		perror("Fallo enviar buffer finalizar umc");
	}
	log_info(logger_manager, "PID %d - Se manda a la umc a ser finalizado",
			pcb_quantum->pcb->pid);
	free(finalizar);
	free(buffer_finalizar);
	free(header_finalizar_umc);

//asignar_pcb_a_cpu(socket_cpu);
}

void actualizar_estado_pcb_y_saco_socket_cpu(t_pcb *pcb, int estado) { //para ready o block serviria
	sem_wait(&mutex_lista_procesos);
	bool busqueda_proceso_logica(t_fila_tabla_procesos * proceso) {
		return (pcb->pid == proceso->pcb->pid);
	}
	t_fila_tabla_procesos *proceso = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)));
	proceso->socket_cpu = NO_ASIGNADO;
	proceso->pcb = pcb;
	proceso->pcb->estado = estado;
	sem_post(&mutex_lista_procesos);
}

void actualizar_pcb_y_ponerlo_en_ready_con_socket_cpu(t_pcb *pcb,
		int socket_cpu) {

	sem_wait(&mutex_lista_procesos);
	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (socket_cpu == proceso->socket_cpu);
	}
	t_fila_tabla_procesos *proceso = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)));
	proceso->socket_cpu = NO_ASIGNADO;
	proceso->pcb = pcb;
	proceso->pcb->estado = READY;
	sem_post(&mutex_lista_procesos);

}

void finalizar_proceso_en_lista_proc_con_socket_cpu(t_pcb * pcb, int socket_cpu) {

	//
	int socket_con = buscar_socket_consola_por_pid(pcb->pid);
	eliminar_proceso_de_lista_procesos_con_pid(pcb->pid);
	enviar_header_completado(socket_con, PROCESO_CONSOLA,
	MENSAJE_FINALIZO_OK);

}

void * queue_pop_pid(t_queue *self, int pid) {
	bool busqueda_proceso_logica(t_pcb * pcb) {
		return (pid == pcb->pid);
	}
	return list_remove_by_condition(self->elements,
			(void*) busqueda_proceso_logica);
}

t_cpu * queue_pop_cpu(t_queue *self, int socket_cpu) {
	bool busqueda_proceso_logica(t_cpu * cpu) {
		return (socket_cpu == cpu->socket_cpu);
	}
	return list_remove_by_condition(self->elements,
			(void*) busqueda_proceso_logica);
}

void agregar_cpu_disponible(int socket_conexion) {
	t_cpu * cpu_nueva = malloc(sizeof(t_cpu));
	cpu_nueva->socket_cpu = socket_conexion;
	sem_wait(&mutex_cola_cpu);
	queue_push(cola_cpus, cpu_nueva);
	log_info(logger_manager, "Se libera el cpu: %i para su uso",
			socket_conexion);
	sem_post(&mutex_cola_cpu);
	sem_post(&cant_cpu);
}

void actualizar_pcb_y_ponerlo_en_exec_con_socket_cpu(t_pcb *pcb, int socket_cpu) {

	sem_wait(&mutex_lista_procesos);
	bool busqueda_proceso_logica(t_fila_tabla_procesos *proceso) {
		return (pcb->pid == proceso->pcb->pid);
	}
	t_fila_tabla_procesos *proceso = (((t_fila_tabla_procesos*) list_find(
			lista_procesos, (void*) busqueda_proceso_logica)));
	log_info(logger_manager, "PID: %d El socket cpu del proceso es: %i",
			pcb->pid, proceso->socket_cpu);
	log_info(logger_manager, "PID: %d El socket consola del proceso es: %i",
			pcb->pid, proceso->socket_consola);
	if (proceso->pcb != NULL) {
		log_info(logger_manager, "PID: %d El estado del pcb es: %i", pcb->pid,
				proceso->pcb->estado);
	}
	proceso->socket_cpu = socket_cpu;
	proceso->pcb->estado = EXEC;
	sem_post(&mutex_lista_procesos);

}

// Monitorea los cambios en el archivo de configuración y actualiza t_config_nucleo cuando ocurra
void monitorear_configuraciones() {

	log_trace(logger_manager,
			"[IONOTIFY] Inicializando minitoreo de archivo de configuraciones...");

	char buffer_eventos[IONOTIFY_BUFFER_EVENT_SIZE]; //Buffer de eventos

	int fd_ionotify = inotify_init();

	if (fd_ionotify < 0)
		log_error(logger_manager,
				"[IONOTIFY] Ocurrio un problema al inicializar");
	else
		log_trace(logger_manager, "[IONOTIFY] Inicializacion correcta.");

	int fd_watch = inotify_add_watch(fd_ionotify, PATH_CONFIGURACIONES_NUCLEO,
	IN_MODIFY); //Se saca  | IN_CREATE | IN_DELETE | IN_MODIFY

	while (1) {

		int cantidad_lectura_de_eventos = read(fd_ionotify, buffer_eventos,
		IONOTIFY_BUFFER_EVENT_SIZE); //Este read es una llamada bloqueante

		if (cantidad_lectura_de_eventos < 0)
			log_error(logger_manager,
					"[IONOTIFY] Ocurrio un problema al leer los eventos en el buffer");

		int offset = 0;

		while (offset < cantidad_lectura_de_eventos) {

			struct inotify_event *evento =
					(struct inotify_event *) &buffer_eventos[offset]; //Obtengo el evento

			if (evento->mask & IN_MODIFY) {
				//log_trace(logger_manager, "[IONOTIFY] El archivo de configuraciones fue modificado, actualizando datos...");

				cargar_nuevas_configuraciones_del_nucleo(
				PATH_CONFIGURACIONES_NUCLEO, configuracion);

				//log_trace(logger_manager, "[IONOTIFY] Las configuraciones fueron actualizadas con exito!");
				//log_trace(logger_manager, "[IONOTIFY] Ahora los valores son los siguientes: QUANTUM=%i, QUANTUM_SLEEP=%i" , configuracion->quantum, configuracion->quantum_sleep);
			}
			offset += sizeof(struct inotify_event) + evento->len;
		}
	}

	//------------ _Liberación de recursos IONOTIFY -----------//
	inotify_rm_watch(fd_ionotify, fd_watch);
	close(fd_ionotify);
	//---------------------------------------------------------//

}

//IONOTIFY: Carga solo los parametros del .ini que deben poderse modificar en tiempo de ejecución según el enunciado (QUANTUM y QUANTUM_SLEEP)
void cargar_nuevas_configuraciones_del_nucleo(char *archivo_configuracion,
		t_config_nucleo *configuracion_nucleo) {

	t_config *configuracion = config_create(archivo_configuracion);

	if (config_has_property(configuracion, "QUANTUM")) {
		int nuevo_quantum = config_get_int_value(configuracion, "QUANTUM");
		if (config_has_property(configuracion, "QUANTUM")
				&& configuracion_nucleo->quantum != nuevo_quantum) {
			log_trace(logger_manager,
					"[IONOTIFY] Se cambia el valor del QUANTUM de %i a %i",
					configuracion_nucleo->quantum, nuevo_quantum);
			configuracion_nucleo->quantum = nuevo_quantum;
		}
	}

	if (config_has_property(configuracion, "QUANTUM_SLEEP")) {
		int nuevo_quantum_sleep = config_get_int_value(configuracion,
				"QUANTUM_SLEEP");
		if (config_has_property(configuracion, "QUANTUM_SLEEP")
				&& configuracion_nucleo->quantum_sleep != nuevo_quantum_sleep) {
			log_trace(logger_manager,
					"[IONOTIFY] Se cambia el valor del QUANTUM_SLEEP de %i a %i",
					configuracion_nucleo->quantum_sleep, nuevo_quantum_sleep);
			configuracion_nucleo->quantum_sleep = nuevo_quantum_sleep;
		}
	}

	config_destroy(configuracion);
}

