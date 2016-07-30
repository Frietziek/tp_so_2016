/*
 ============================================================================
 Name        : cpu.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cpu.h"

// Primirivas AnSISOP
AnSISOP_funciones functions = { .AnSISOP_definirVariable =
		ansisop_definir_variable, .AnSISOP_obtenerPosicionVariable =
		ansisop_obtener_posicion_variable, .AnSISOP_dereferenciar =
		ansisop_derefenciar, .AnSISOP_asignar = ansisop_asignar,
		.AnSISOP_obtenerValorCompartida = ansisop_obtener_valor_compartida,
		.AnSISOP_asignarValorCompartida = ansisop_asignar_valor_compartida,
		.AnSISOP_irAlLabel = ansisop_ir_a_label, .AnSISOP_retornar =
				ansisop_retornar, .AnSISOP_llamarConRetorno =
				ansisop_llamar_con_retorno, .AnSISOP_finalizar =
				ansisop_finalizar, .AnSISOP_imprimir = ansisop_imprimir,
		.AnSISOP_imprimirTexto = ansisop_imprimir_texto,
		.AnSISOP_entradaSalida = ansisop_entrada_salida };
AnSISOP_kernel kernel_functions = { .AnSISOP_wait = ansisop_wait,
		.AnSISOP_signal = ansisop_signal };

int main(void) {

	logger_manager = log_create("cpu.log", "CPU", true, LOG_LEVEL_TRACE); // Creo archivo de log
	inicio_seniales_semaforos();

	// Cargo configuraciones desde archivo ini
	t_config_cpu *configuracion = malloc(sizeof(t_config_cpu));
	configuracion->ip_nucleo = malloc(40);
	configuracion->ip_umc = malloc(40);
	carga_configuracion_cpu("config.cpu.ini", configuracion);
	log_trace(logger_manager, "Proceso CPU creado.");

	socket_nucleo = conecto_con_nucleo(configuracion);

	socket_umc = conecto_con_umc(configuracion);

	cpu_ocupada = 0;

	sem_wait(&s_cpu_finaliza);

	cierro_cpu(configuracion);

	return EXIT_SUCCESS;
}

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion_cpu) {
	t_config *configuracion = config_create(archivo);
	if (config_has_property(configuracion, "IP_NUCLEO")) {
		strcpy(configuracion_cpu->ip_nucleo,
				config_get_string_value(configuracion, "IP_NUCLEO"));
	}
	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracion_cpu->puerto_nucleo = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
	}
	if (config_has_property(configuracion, "IP_UMC")) {
		strcpy(configuracion_cpu->ip_umc,
				config_get_string_value(configuracion, "IP_UMC"));
	}
	if (config_has_property(configuracion, "PUERTO_UMC")) {
		configuracion_cpu->puerto_umc = config_get_int_value(configuracion,
				"PUERTO_UMC");
	}
	config_destroy(configuracion);
}

void inicio_seniales_semaforos() {
	// Funcion para atender seniales
	signal(SIGUSR1, atender_seniales);
	signal(SIGINT, atender_seniales);
	// Inicio semaforos
	sem_init(&s_cpu_finaliza, 0, 0); // Semaforo para la finalizacion de CPU
	sem_init(&s_codigo, 0, 0); // Semaforo para pedido de lectura de codigo en UMC
	sem_init(&s_instruccion_finalizada, 0, 0); // Semaforo para indicar inst fin con resp de UMC / Nucleo
	sem_init(&s_cambio_proceso, 0, 0); // Semaforo para la confirmacion de cambio de Proceso de UMC
	sem_init(&s_variable_stack, 0, 0); // Semaforo para pedido de lectura de variable en UMC
	sem_init(&s_variable_compartida, 0, 0); // Semaforo para pedido de lectura de var comp en Nucleo
	sem_init(&s_matar_cpu, 0, 0); // Semaforo para matar CPU con SIGUSR1
	sem_init(&s_escribir_pagina, 0, 0); // Para cuando pido escribir una pagina en UMC
	sem_init(&s_envio_pcb, 0, 0); // Para cuando envio el PCB al Nucleo
	// Inicio atributos de hilos
	pthread_attr_init(&attr_io);
	pthread_attr_init(&attr_instruccion);
	pthread_attr_setdetachstate(&attr_io, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr_instruccion, PTHREAD_CREATE_DETACHED);
	// Reservo memoria para Qunatum - PCB
	pcb_quantum = malloc(sizeof(t_pcb_quantum));
	// Inicio variable para instruccion wait de ansisop
	wait_nucleo = 0;
	// Inicio variable para que no mate al proceso
	matar_proceso = 0;
	// Inicio variable de Excepcion de UMC
	excepcion_umc = 0;
	// Inicio variable para matar CPU
	matar_cpu = 0;
	// Inicio variable para E/S
	entrada_salida = 0;
}

void cierro_cpu(t_config_cpu* configuracion) {
	log_trace(logger_manager, "Cerrando CPU.");
	log_destroy(logger_manager);
	sem_destroy(&s_cpu_finaliza);
	sem_destroy(&s_codigo);
	sem_destroy(&s_instruccion_finalizada);
	sem_destroy(&s_cambio_proceso);
	sem_destroy(&s_variable_stack);
	sem_destroy(&s_variable_compartida);
	sem_destroy(&s_matar_cpu);
	free(valor_pagina);
	libero_pcb();
	free(pcb_quantum);
	free(configuracion->ip_nucleo);
	free(configuracion->ip_umc);
	free(configuracion);
	if (socket_umc >= 0) {
		close(socket_umc);
	}
	if (socket_nucleo >= 0) {
		close(socket_nucleo);
	}
}

int conecto_con_nucleo(t_config_cpu* configuracion) {
	int socket_servidor = conectar_servidor(configuracion->ip_nucleo,
			configuracion->puerto_nucleo, &atender_nucleo);
	if (socket_servidor > 0) {
		log_trace(logger_manager, "CPU conectado con Nucleo.");
		handshake_cpu_nucleo(socket_servidor);
	} else {
		log_warning(logger_manager, "Error al conectarse con Nucleo.");
	}
	return socket_servidor;
}

int conecto_con_umc(t_config_cpu* configuracion) {
	int socket_servidor = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc, &atender_umc);
	if (socket_servidor > 0) {
		log_trace(logger_manager, "CPU conectado con UMC.");
		handshake_cpu_umc(socket_servidor);
	} else {
		log_warning(logger_manager, "Error al conectarse con UMC.");
	}
	return socket_servidor;
}

void atender_seniales(int signum) {
	switch (signum) {
	case SIGINT:	//control C

		log_trace(logger_manager, "Se recibio senial de cierre de proceso.");
		matar_cpu = 1;
		if (cpu_ocupada == 1) {
			sem_wait(&s_matar_cpu);
		} else {
			envio_header_a_proceso(socket_nucleo, PROCESO_NUCLEO,
			MENSAJE_SIGINT, "Fallo al enviar Desconexion al Nucleo.");
		}
		cambio_proceso_activo(-1);
		sem_post(&s_cpu_finaliza);
		break;
	case SIGUSR1:
		log_trace(logger_manager, "Se recibio senial de cierre de proceso.");
		matar_cpu = 1;
		if (cpu_ocupada == 1) {
			sem_wait(&s_matar_cpu);
		} else {
			envio_header_a_proceso(socket_nucleo, PROCESO_NUCLEO,
			MENSAJE_DESCONEXION_CPU, "Fallo al enviar Desconexion al Nucleo.");
		}
		cambio_proceso_activo(-1);
		sem_post(&s_cpu_finaliza);
		break;
	default:
		log_trace(logger_manager, "Se recibio una senial desconocida.");
		break;
	}
}

// Funciones CPU - UMC
void atender_umc(t_paquete *paquete, int socket_conexion) {
	switch (paquete->header->id_mensaje) {
	case RESPUESTA_HANDSHAKE:
		log_info(logger_manager, "Handshake recibido de UMC.");
		respuesta_handshake_cpu_umc(paquete->payload);
		break;
	case RESPUESTA_LEER_PAGINA:
		respuesta_leer_pagina(paquete->payload);
		break;
	case RESPUESTA_ESCRIBIR_PAGINA:
		log_info(logger_manager, "Se escribio una pagina en UMC.");
		sem_post(&s_escribir_pagina);
		break;
	case RESPUESTA_CAMBIO_PROCESO_ACTIVO:
		log_info(logger_manager, "La UMC confirmo cambio de proceso: %i",
				pcb_quantum->pcb->pid);
		sem_post(&s_cambio_proceso);
		break;
	case ERROR_HANDSHAKE:
		log_error(logger_manager, "Error en Handshake con UMC.");
		break;
	case ERROR_LEER_PAGINA:
		log_error(logger_manager, "Error en lectura de pagina de UMC.");
		excepcion_umc = 1;
		sem_post(&s_codigo);
		break;
	case ERROR_ESCRIBIR_PAGINA:
		log_error(logger_manager, "Error en escritura de pagina en UMC.");
		excepcion_umc = 1;
		sem_post(&s_escribir_pagina);
		break;
	default:
		log_warning(logger_manager, "Comando no reconocido de UMC.");
		break;
	}
}

void handshake_cpu_umc(int socket_servidor) {
	envio_header_a_proceso(socket_servidor, PROCESO_UMC, MENSAJE_HANDSHAKE,
			"Fallo al enviar Handshake a la UMC.");
}

void respuesta_handshake_cpu_umc(void *buffer) {
	t_pagina_tamanio *pagina = malloc(sizeof(t_pagina_tamanio));
	deserializar_pagina_tamanio(buffer, pagina);
	tamanio_pagina = pagina->tamanio;
	log_info(logger_manager, "Se cargo el tamanio de la pagina: %i",
			tamanio_pagina);
	free(pagina);
}

void respuesta_leer_pagina(void *buffer) {
	t_pagina_pedido_completa *pagina = malloc(sizeof(t_pagina_pedido_completa));
	deserializar_pagina_pedido_completa(buffer, pagina);
	log_info(logger_manager, "Recibo de UMC pag %i offset %i tamanio %i",
			pagina->pagina, pagina->offset, pagina->tamanio);
	valor_pagina = realloc(valor_pagina, pagina->tamanio);
	size_pagina = pagina->tamanio;
	memcpy(valor_pagina, pagina->valor, pagina->tamanio);
	if (pagina_es_codigo) {
		sem_post(&s_codigo);
	} else {
		sem_post(&s_variable_stack);
	}
	free(pagina->valor);
	free(pagina);
}

// Funciones CPU - Nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion) {
	switch (paquete->header->id_mensaje) {
	case RESPUESTA_HANDSHAKE:
		log_info(logger_manager, "Handshake recibido de Nucleo.");
		break;
	case MENSAJE_PCB_NUCLEO:
		log_info(logger_manager, "Recibo PCB de Nucleo.");
		recibo_PCB(paquete->payload);
		break;
	case MENSAJE_MATAR:
		log_info(logger_manager, "Recibo matar de Nucleo.");
		matar_proceso = 1;
		break;
	case RESPUESTA_LEER_COMPARTIDA:
		log_info(logger_manager, "Recibo variable compartida de Nucleo.");
		respuesta_leer_compartida(paquete->payload);
		break;
	case RESPUESTA_ESCRIBIR_COMPARTIDA:
		log_info(logger_manager, "Se escribio variable compartida en Nucleo.");
		break;
	case RESPUESTA_IMPRIMIR:
		log_info(logger_manager, "Se imprimio variable en Nucleo.");
		break;
	case RESPUESTA_IMPRIMIR_TEXTO:
		log_info(logger_manager, "Se imprimio texto en Nucleo.");
		break;
	case RESPUESTA_ENTRADA_SALIDA:
		log_info(logger_manager, "Recibi respuesta de IO de Nucleo.");
		break;
	case RESPUESTA_WAIT:
		log_info(logger_manager, "Recibi wait y envio PCB a Nucleo.");
		sem_post(&s_instruccion_finalizada);
		break;
	case RESPUESTA_SEGUI_RAFAGA:
		log_info(logger_manager, "Continuo con mi rafaga actual.");
		wait_nucleo = 0;
		sem_post(&s_instruccion_finalizada);
		break;
	case RESPUESTA_SIGNAL:
		log_info(logger_manager, "Recibi respuesta de signal de Nucleo.");
		break;
	case RESPUESTA_QUANTUM:
		log_info(logger_manager, "Recibi respuesta de quantum de Nucleo.");
		break;
	case RESPUESTA_MATAR_CPU:
		log_info(logger_manager, "Recibi respuesta de Nucleo para matar CPU.");
		break;
	case ERROR_HANDSHAKE:
		log_error(logger_manager, "Error en Handshake con el Nucleo.");
		break;
	case ERROR_LEER_COMPARTIDA:
		log_error(logger_manager, "Error al leer var compartida de Nucleo.");
		break;
	case ERROR_ESCRIBIR_COMPARTIDA:
		log_error(logger_manager, "Error al escribir var comp de Nucleo.");
		break;
	case ERROR_IMPRIMIR:
		log_error(logger_manager, "Error al imprimir variable en Nucleo.");
		break;
	case ERROR_IMPRIMIR_TEXTO:
		log_error(logger_manager, "Error al imprimir texto en Nucleo.");
		break;
	case ERROR_ENTRADA_SALIDA:
		log_error(logger_manager, "Error al recibir IO de Nucleo.");
		break;
	case ERROR_WAIT:
		log_error(logger_manager, "Error al recibir Wait de Nucleo.");
		break;
	case ERROR_SIGNAL:
		log_error(logger_manager, "Error al recibir Signal de Nucleo.");
		break;
	case ERROR_QUANTUM:
		log_error(logger_manager, "Error al recibir quantum de Nucleo.");
		break;
	case ERROR_MATAR_CPU:
		log_error(logger_manager, "Error al recibir matar CPU de Nucleo.");
		break;
	default:
		log_warning(logger_manager, "Comando no reconocido de Nucleo.");
		break;
	}
}

void handshake_cpu_nucleo(int socket_servidor) {
	envio_header_a_proceso(socket_servidor, PROCESO_NUCLEO, MENSAJE_HANDSHAKE,
			"Fallo al enviar Handshake al Nucleo.");
}

void recibo_PCB(void *buffer) {
	if (pcb_quantum->pcb != NULL) {
		libero_pcb();
	}
	cpu_ocupada = 1;
	deserializar_pcb_quantum(buffer, pcb_quantum);
	cambio_proceso_activo(pcb_quantum->pcb->pid);
	sem_wait(&s_cambio_proceso);
	log_info(logger_manager, "Se ejecutara el PID: %d con QUANTUM: %d",
			pcb_quantum->pcb->pid, pcb_quantum->quantum);
	pthread_create(&hilo_instruccion, &attr_instruccion,
			(void*) ejecuto_instrucciones, NULL);
}

void enviar_PCB(int id_mensaje) {
	t_buffer *buffer = serializar_pcb_quantum(pcb_quantum);
	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO, id_mensaje,
			"Fallo al enviar PCB a Nucleo", buffer);
	log_info(logger_manager, "Se envio el PCB %i al nucleo.",
			pcb_quantum->pcb->pid);
	cpu_ocupada = 0;

	if (id_mensaje == MENSAJE_ENTRADA_SALIDA_PCB) {
		sem_post(&s_envio_pcb);
		sem_post(&s_envio_pcb);
	}

	if (id_mensaje == MENSAJE_MATAR_CPU) {
		sem_post(&s_matar_cpu);
	}

	free(buffer->contenido_buffer);
	free(buffer);
}

void cambio_proceso_activo(int id_programa) {
	t_programa *p_programa = malloc(sizeof(t_programa));
	p_programa->id_programa = id_programa;
	t_buffer *buffer = serializar_programa(p_programa);

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC,
	MENSAJE_CAMBIO_PROCESO_ACTIVO,
			"Fallo al enviar cambio de proceso activo a UMC.", buffer);

	free(p_programa);
	free(buffer->contenido_buffer);
	free(buffer);
}

void ejecuto_instrucciones() {
	fin_proceso = 0;
	while (pcb_quantum->quantum != FIN_QUANTUM && !fin_proceso && !wait_nucleo
			&& !matar_proceso && !excepcion_umc && !entrada_salida && !matar_cpu) {

		char *instruccion = devuelve_instruccion_a_ejecutar();

		if (excepcion_umc) {
			log_warning(logger_manager, "Se produjo una excepcion en la UMC");
			break;
		}

		log_info(logger_manager, "Instruccion a ejecutar: %s", instruccion);

		char *instruccion_a_ejecutar = strdup(instruccion);

		analizadorLinea(instruccion_a_ejecutar, &functions, &kernel_functions);

		free(instruccion);
		free(instruccion_a_ejecutar);

		sem_wait(&s_instruccion_finalizada);

		sem_init(&s_instruccion_finalizada, 0, 0);

		log_info(logger_manager, "Instruccion terminada.");

		++pcb_quantum->pcb->pc;
		--pcb_quantum->quantum;
	}

	int id_mensaje = devuelve_id_mensaje();

	enviar_PCB(id_mensaje);
	pthread_attr_destroy(&attr_instruccion);
}

char *devuelve_instruccion_a_ejecutar() {
	t_intructions *instruccion_deserializada = deserializo_instruccion(
			pcb_quantum->pcb->pc);
	int tamanio_instruccion = instruccion_deserializada->offset;
	char *instruccion = malloc(sizeof(char) * tamanio_instruccion);
	int cantidad_paginas = calcula_paginas_instruccion();
	log_info(logger_manager, "La instruccion empieza %i con tamanio %i",
			instruccion_deserializada->start,
			instruccion_deserializada->offset);
	log_info(logger_manager, "La instruccion esta en %i pagina(s)",
			cantidad_paginas);
	int pagina;
	int posicion_instruccion = 0;
	for (pagina = 0; pagina < cantidad_paginas; ++pagina) {
		leo_instruccion_desde_UMC(pagina);
		sem_wait(&s_codigo);
		if (excepcion_umc) {
			break;
		}
		memcpy(instruccion + posicion_instruccion, valor_pagina, size_pagina);
		posicion_instruccion += size_pagina;
	}
	instruccion[tamanio_instruccion - 1] = '\0';

	return instruccion;

}

int devuelve_id_mensaje() {
	int id_mensaje;
	if (entrada_salida) {
		id_mensaje = MENSAJE_ENTRADA_SALIDA_PCB;
		log_info(logger_manager,
				"Se envia PCB al nucleo por MENSAJE_ENTRADA_SALIDA_PCB");
		entrada_salida = 0;
	} else if (wait_nucleo) {
		id_mensaje = MENSAJE_WAIT_PCB;
		log_info(logger_manager, "Se envia PCB al nucleo por MENSAJE_WAIT_PCB");
		wait_nucleo = 0;
	} else if (matar_proceso) {
		id_mensaje = RESPUESTA_MATAR;
		log_info(logger_manager, "Se envia PCB al nucleo por RESPUESTA_MATAR");
		cambio_proceso_activo(0);
		matar_proceso = 0;
	} else if (fin_proceso) {
		id_mensaje = MENSAJE_PROGRAMA_FINALIZADO;
		log_info(logger_manager,
				"Se envia PCB al nucleo por MENSAJE_PROGRAMA_FINALIZADO");
	} else if (excepcion_umc) {
		ansisop_imprimir_texto("La UMC respondio con una Excepcion.");
		id_mensaje = MENSAJE_EXCEPCION_UMC;
		log_info(logger_manager,
				"Se envia PCB al nucleo por MENSAJE_EXCEPCION_UMC");
		excepcion_umc = 0;
	} else if (matar_cpu) {
		id_mensaje = MENSAJE_MATAR_CPU;
		log_info(logger_manager,
				"Se envia PCB al nucleo por MENSAJE_MATAR_CPU");
		matar_cpu = 0;
	} else {
		id_mensaje = MENSAJE_QUANTUM;
		log_info(logger_manager, "Finaliza Quantum.");
	}

	return id_mensaje;
}

int calcula_paginas_instruccion() {
	t_intructions *instruccion = deserializo_instruccion(pcb_quantum->pcb->pc);
	int pagina_start = calcula_pagina(instruccion->start);
	int pagina_tamanio = calcula_pagina(
			instruccion->start + instruccion->offset);
	return pagina_tamanio - pagina_start + 1;
}

t_intructions *deserializo_instruccion(int pc) {
	return pcb_quantum->pcb->instrucciones_serializadas + pc;
}

void envio_excepcion_nucleo(int id_excepcion, char *mensaje_excepcion) {
	t_texto *p_texto = malloc(sizeof(t_texto));
	p_texto->texto = mensaje_excepcion;
	t_buffer *buffer = serializar_texto(p_texto);

	envio_buffer_a_proceso(socket_nucleo, PROCESO_NUCLEO, id_excepcion,
			"Fallo al enviar excepcion al Nucleo.", buffer);

	free(p_texto);
	free(buffer->contenido_buffer);
	free(buffer);
}

void leo_instruccion_desde_UMC(int pagina) {
	t_pagina_pedido *p_pagina = malloc(sizeof(t_pagina_pedido));
	t_intructions *instruccion = deserializo_instruccion(pcb_quantum->pcb->pc);
	p_pagina->pagina = calcula_pagina(instruccion->start) + pagina;
	p_pagina->offset = calcula_offset_instruccion(instruccion->start, pagina);
	p_pagina->tamanio = calcula_tamanio_instruccion(instruccion, p_pagina,
			pagina);
	t_buffer *buffer = serializar_pagina_pedido(p_pagina);

	pagina_es_codigo = 1;
	log_info(logger_manager, "Pido a UMC pag %i offset %i tamanio %i",
			p_pagina->pagina, p_pagina->offset, p_pagina->tamanio);

	envio_buffer_a_proceso(socket_umc, PROCESO_UMC, MENSAJE_LEER_PAGINA,
			"Fallo al enviar lectura de pagina a UMC.", buffer);

	free(p_pagina);
	free(buffer->contenido_buffer);
	free(buffer);
}

void respuesta_leer_compartida(void *buffer) {
	t_variable_completa *compartida = malloc(sizeof(t_variable_completa));
	deserializar_variable_completa(buffer, compartida);
	valor_pagina = realloc(valor_pagina, sizeof(int));
	memcpy(valor_pagina, &(compartida->valor), sizeof(int));
	free(compartida->nombre);
	free(compartida);
	sem_post(&s_variable_compartida);
}

void libero_pcb() {
	/*t_indice_stack* indice_stack = pcb_quantum->pcb->indice_stack;
	 int i_stack;
	 for (i_stack = 0; i_stack < pcb_quantum->pcb->stack_size; ++i_stack) {
	 indice_stack += i_stack;
	 int i_variables;
	 for (i_variables = 0; i_variables < indice_stack->cantidad_variables;
	 ++i_variables) {
	 t_variables_stack* indice_variables = indice_stack->variables;
	 indice_variables += i_variables;
	 free(indice_variables->posicion_memoria);
	 }
	 free(indice_stack->posicion_variable_retorno);
	 free(indice_stack->variables);
	 }*/
	free(pcb_quantum->pcb->instrucciones_serializadas);
	free(pcb_quantum->pcb->indice_stack);
	free(pcb_quantum->pcb->etiquetas);
	free(pcb_quantum->pcb);
}

int calcula_tamanio_instruccion(t_intructions *instruccion,
		t_pagina_pedido *p_pagina, int pagina) {
	int tamanio =
			(instruccion_en_una_pagina(instruccion, p_pagina)) ?
					instruccion->offset : tamanio_pagina - p_pagina->offset;
	return (pagina == 0) ?
			tamanio :
			instruccion->offset
					- (tamanio_pagina - calcula_offset(instruccion->start));
}
