/*
 ============================================================================
 Name        : umc.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "umc.h"

int socket_swap;
int socket_nucleo;
void *memoria_principal;
t_lista_algoritmo * listas_algoritmo;
t_list *lista_de_marcos, *lista_paginas_tlb, *lista_tablas,
		*lista_tabla_entradas, *lista_cpus, *lista_procesos_activos;
t_config_umc *configuracion;
FILE * dump_file;
t_log * log_umc;
pthread_t thread_consola;

void * buffer_programas[CANT_TABLAS_MAX]; //se busca por pid
static pthread_mutex_t mutex_lista_tabla_entradas = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_lista_paginas_tlb = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_lista_cpus = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
	//creo archivo log

	log_umc = log_create("umc.out", "Proceso_UMC", 1, LOG_LEVEL_INFO);

	// Estructura de configuracion de la UMC
	configuracion = malloc(sizeof(t_config_umc));
	carga_configuracion_UMC("config.umc.ini", configuracion);

	// Se crea el bloque de la memoria principal
	memoria_principal = calloc(configuracion->marcos,
			configuracion->marco_size);
	log_info(log_umc, "Iniciando UMC...  Memoria disponible: %d bytes",
			configuracion->marcos * configuracion->marco_size);

	//iniciar listas
	crear_listas();

	//creo los marcos
	crear_marcos();

	//Para probar
	//buffer_programas = malloc(sizeof(CANT_TABLAS_MAX));

	//Inicio el archivo dump
	dump_file = txt_open_for_append("./dump.out");
	txt_write_in_file(dump_file, "\n\n NUEVO INICIO DE UMC \n\n");

	// Se realiza la conexión con el swap
	socket_swap = conecto_con_swap(configuracion);

	// Inicio servidor UMC
	pthread_create(&thread_consola, NULL, (void*) menu_principal, NULL);

	creo_servidor_umc(configuracion);
	//getchar(); //pausa
	pthread_join(thread_consola, NULL);

	free(configuracion);

	close(socket_swap);
	free(memoria_principal);

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
	if (config_has_property(configuracion, "ALGORITMO")) {
		configuracionUMC->algoritmo = config_get_string_value(configuracion,
				"ALGORITMO");
	}
	free(configuracion);
}

t_configuracion_servidor* creo_servidor_umc(t_config_umc* configuracion) {
	t_configuracion_servidor* servidor_umc = malloc(
			sizeof(t_configuracion_servidor));
	servidor_umc->puerto = configuracion->puerto;
	servidor_umc->funcion = &atender_peticiones;
	servidor_umc->parametros_funcion = configuracion;
	crear_servidor(servidor_umc);
	log_info(log_umc, "Servidor UMC disponible para escuchar conexiones");
	return servidor_umc;
}

int conecto_con_swap(t_config_umc* configuracion) {
	int socket_servidor;
	if ((socket_servidor = conectar_servidor(configuracion->ip_swap,
			configuracion->puerto_swap, &atender_swap)) > 0) {
		log_info(log_umc, "Se conecta con el SWAP de manera satisfactoria");
		handshake_umc_swap(socket_servidor, configuracion);
	} else {
		log_error(log_umc, "Error de conexión con el SWAP\n");
		log_info(log_umc, "Se finaliza la UMC");
		exit(1);
	}
	return socket_servidor;
}

void * menu_principal() {
	int comando;
	// Comandos ingresados de la consola de UMC
	while (1) {
		printf("Ingrese uno de los siguientes comandos para continuar:\n");
		printf("1 - Cambiar retardo de la consola UMC\n");
		printf("2 - Generar reporte y archivo Dump\n");
		printf("3 - Limpiar contenido de LTB o paginas\n");
		printf("4 - Mostrar estado TLB\n");
		scanf("%d", &comando);
		switch (comando) {
		case RETARDO:
			cambiar_retardo();
			break;
		case DUMP:
			printf(
					"Especificar el pid del proceso o presione 0 para todos los procesos\n");
			scanf("%d", &comando);
			generar_dump(comando);
			break;
		case FLUSH:
			printf("1 - Limpiar la TLB\n");
			printf("2 - Marcar todas las paginas como modificadas\n");
			scanf("%d", &comando);
			switch (comando) {
			case TLB:
				flush_tlb();
				break;
			case MEMORY:
				marcar_todas_modificadas();
				break;
			default:
				printf("Comando no reconocido\n");
				break;
			}
			break;
		case ESTADO_TLB:
			test_tlb();
			break;
		default:
			printf("Comando no reconocido\n");
			break;
		}
	}
}

void atender_peticiones(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion) {
	switch (paquete->header->id_proceso_emisor) {
	case PROCESO_CPU:
		atender_cpu(paquete, socket_conexion, configuracion);
		break;
	case PROCESO_NUCLEO:
		atender_nucleo(paquete, socket_conexion, configuracion);
		break;
	default:
		log_error(log_umc,
				"Proceso ID:%d No tiene permisos para comunicarse con la UMC",
				paquete->header->id_proceso_emisor);
		break;
	}
}

void atender_cpu(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		log_info(log_umc, "Handshake recibido de CPU");
		handshake_umc_cpu(socket_conexion, configuracion);
		break;
	case MENSAJE_LEER_PAGINA:
		log_info(log_umc, "Recibido mensaje leer pagina");
		leer_pagina(paquete->payload, socket_conexion, configuracion);
		break;
	case MENSAJE_ESCRIBIR_PAGINA:
		log_info(log_umc, "Recibido mensaje escribir pagina");
		escribir_pagina(paquete->payload, socket_conexion);
		break;
	case MENSAJE_CAMBIO_PROCESO_ACTIVO:
		log_info(log_umc,
				"========= Cambio de proceso activo =========");
		cambiar_proceso_activo(paquete->payload, socket_conexion);
		break;
	default:
		log_error(log_umc, "Id mensaje enviado por el CPU no reconocido: %d",
				paquete->header->id_mensaje);
		break;
	}
}

void atender_nucleo(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		log_info(log_umc, "Handshake recibido de Nucleo.");
		handshake_umc_nucleo(socket_conexion, configuracion);
		break;
	case MENSAJE_INICIALIZAR_PROGRAMA:
		log_info(log_umc, "Solicitud de un nuevo programa.");
		iniciar_programa(paquete->payload);
		break;
	case MENSAJE_MATAR_PROGRAMA:
		log_info(log_umc,
				"Solicitud recibida del nucleo: MENSAJE_MATAR_PROGRAMA");
		finalizar_programa(paquete->payload, paquete->header->id_mensaje);
		break;
	case MENSAJE_FINALIZAR_PROGRAMA:
		log_info(log_umc,
				"Solicitud recibida del nucleo: MENSAJE_FINALIZAR_PROGRAMA");
		finalizar_programa(paquete->payload, paquete->header->id_mensaje);
		break;
	default:
		log_error(log_umc, "Id mensaje enviado por el NUCLEO no reconocido: %d",
				paquete->header->id_mensaje);
		break;
	}
}

void atender_swap(t_paquete *paquete, int socket_conexion) {
	switch (paquete->header->id_mensaje) {
	case REPUESTA_HANDSHAKE:
		log_info(log_umc, "Recibo de SWAP :REPUESTA_HANDSHAKE");
		respuesta_handshake_umc_swap();
		break;
	case RESPUESTA_LEER_PAGINA:
		log_info(log_umc, "Recibo de SWAP :RESPUESTA_LEER_PAGINA");
		respuesta_leer_pagina(paquete->payload, RESPUESTA_LEER_PAGINA);
		break;
	case RESPUESTA_ESCRIBIR_PAGINA:
		log_info(log_umc, "Recibo de SWAP :RESPUESTA_ESCRIBIR_PAGINA");
		respuesta_escribir_pagina(paquete->payload, RESPUESTA_ESCRIBIR_PAGINA);
		break;
	case RESPUESTA_ESCRIBIR_PAGINA_NUEVA:
		log_info(log_umc, "Recibo de SWAP :RESPUESTA_ESCRIBIR_PAGINA_NUEVA");
		respuesta_escribir_pagina_nueva(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case RESPUESTA_INICIAR_PROGRAMA:
		log_info(log_umc, "Recibo de SWAP :RESPUESTA_INICIAR_PROGRAMA");
		respuesta_iniciar_programa(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case RESPUESTA_FINALIZAR_PROGRAMA:
		log_info(log_umc, "Recibo de SWAP :RESPUESTA_FINALIZAR_PROGRAMA");
		respuesta_finalizar_programa(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR:
		log_info(log_umc,
				"Recibo de SWAP :RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR");
		respuesta_leer_pagina_para_escribir(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case ERROR_INICIAR_PROGRAMA:
		log_info(log_umc, "Recibo de SWAP :ERROR_INICIAR_PROGRAMA");
		respuesta_iniciar_programa(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case ERROR_LEER_PAGINA:
		log_info(log_umc, "Recibo de SWAP :ERROR_LEER_PAGINA");
		respuesta_leer_pagina(paquete->payload, paquete->header->id_mensaje);
		break;
	case ERROR_ESCRIBIR_PAGINA:
		log_info(log_umc, "Recibo de SWAP :ERROR_ESCRIBIR_PAGINA");
		respuesta_escribir_pagina(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case ERROR_ESCRIBIR_PAGINA_NUEVA:
		log_info(log_umc, "Recibo de SWAP :ERROR_ESCRIBIR_PAGINA_NUEVA");
		respuesta_escribir_pagina_nueva(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case ERROR_FINALIZAR_PROGRAMA:
		log_info(log_umc, "Recibo de SWAP :ERROR_FINALIZAR_PROGRAMA");
		respuesta_finalizar_programa(paquete->payload,
				paquete->header->id_mensaje);
		break;
	case ERROR_LEER_PAGINA_PARA_ESCRIBIR:
		log_info(log_umc, "Recibo de SWAP :ERROR_LEER_PAGINA_PARA_ESCRIBIR");
		respuesta_leer_pagina_para_escribir(paquete->payload,
				paquete->header->id_mensaje);
		break;
	default:
		log_error(log_umc, "Id mensaje enviado por el SWAP no reconocido: %d",
				paquete->header->id_mensaje);
		break;
	}
}

void handshake_umc_swap(int socket_servidor, t_config_umc *configuracion) {
	handshake_proceso(socket_servidor, configuracion, PROCESO_SWAP,
	MENSAJE_HANDSHAKE);
}

void respuesta_handshake_umc_swap() {
	log_info(log_umc, "Handshake de Swap confirmado");
}

void iniciar_programa(void* buffer) {
	t_programa_completo *programa = malloc(sizeof(t_programa_completo));
	deserializar_programa_completo(buffer, programa);

	int i;
	t_fila_tabla_pagina * tabla_paginas = (t_fila_tabla_pagina *) malloc(
			programa->paginas_requeridas * sizeof(t_fila_tabla_pagina));

	listas_algoritmo[programa->id_programa].lista_paginas_mp = list_create();
	listas_algoritmo[programa->id_programa].puntero = 0;

	//armo la tabla:
	for (i = 0; i < programa->paginas_requeridas; i++) {

		tabla_paginas[i].frame = 0;
		tabla_paginas[i].modificado = 0;
		tabla_paginas[i].pid = programa->id_programa;
		tabla_paginas[i].presencia = 0;
		tabla_paginas[i].uso = 0;
		tabla_paginas[i].numero_pagina = i;
		//list_add(listas_algoritmo[programa->id_programa].lista_paginas_mp,(tabla_paginas + i));
	}
	guardar_cant_entradas(programa->id_programa, programa->paginas_requeridas,
			strlen(programa->codigo));

	list_replace(lista_tablas, programa->id_programa, tabla_paginas); // lista de tablas. El index va a coincidir con el pid
	// ejemplo: t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas, 1); -> me retorna la tabla del programa con pid 1

	//el puntero al contenido del programa lo guardo en un buffer
	buffer_programas[programa->id_programa] = programa->codigo;

	t_header *header_swap = malloc(sizeof(t_header));
	header_swap->id_proceso_emisor = PROCESO_UMC;
	header_swap->id_proceso_receptor = PROCESO_SWAP;
	header_swap->id_mensaje = MENSAJE_INICIAR_PROGRAMA;

	t_programa_nuevo *programa_swap = malloc(sizeof(t_programa_nuevo));
	programa_swap->id_programa = programa->id_programa;
	programa_swap->paginas_requeridas = programa->paginas_requeridas;

	t_buffer *payload_swap = serializar_programa_nuevo(programa_swap);

	header_swap->longitud_mensaje = payload_swap->longitud_buffer;
	log_info(log_umc,
			"Se envia a SWAP una solicitud de reserva de espacio para un nuevo programa - PID:%d",
			programa->id_programa);

	if (enviar_buffer(socket_swap, header_swap, payload_swap)
			< sizeof(header_swap) + payload_swap->longitud_buffer) {
		log_error(log_umc,
				"Fallo la comunicacion con SWAP para iniciar el programa PID:%d",
				programa->id_programa);
	}

	free(header_swap);
	free(programa_swap);
	free(payload_swap);
}

void respuesta_iniciar_programa(void *buffer, int id_mensaje) {

	t_programa_nuevo * programa = malloc(sizeof(t_programa_nuevo));
	deserializar_programa_nuevo(buffer, programa);
	t_programa * programa_id = malloc(sizeof(t_programa));
	programa_id->id_programa = programa->id_programa;

	if (id_mensaje == RESPUESTA_INICIAR_PROGRAMA) {
		mandar_a_swap(programa->id_programa, 0, MENSAJE_ESCRIBIR_PAGINA_NUEVA);	//al ser nuevo escribe en la pagina cero
		list_add(lista_procesos_activos, programa_id);
	} else if (id_mensaje == ERROR_INICIAR_PROGRAMA) {
		t_header * header_nucleo = malloc(sizeof(t_header));
		header_nucleo->id_proceso_emisor = PROCESO_UMC;
		header_nucleo->id_proceso_receptor = PROCESO_NUCLEO;
		t_buffer * payload_nucleo = serializar_programa(programa_id);
		header_nucleo->longitud_mensaje = payload_nucleo->longitud_buffer;
		log_error(log_umc, "El SWAP fallo al reservar el nuevo programa PID:%d",
				programa_id->id_programa);
		header_nucleo->id_mensaje = ERROR_INICIALIZAR_PROGRAMA;
		//eliminar_programa_nuevo_en_buffer(programa->id_programa);
		free(buffer_programas[programa->id_programa]);

		if (enviar_buffer(socket_nucleo, header_nucleo, payload_nucleo)
				< sizeof(t_header) + payload_nucleo->longitud_buffer) {
			log_error(log_umc, "Error de comunicacion con el Nucleo");
		}
		log_info(log_umc, "se responde al nucleo - Iniciar programa OK");
		free(header_nucleo);
		free(payload_nucleo);
		free(programa_id);
	}
}

void leer_pagina(void *buffer, int socket_conexion, t_config_umc *configuracion) {
	t_pagina_pedido *pagina = malloc(sizeof(t_pagina_pedido));
	deserializar_pagina_pedido(buffer, pagina);
	bool es_cpu(t_cpu *elemento) {
		return (elemento->socket_cpu == socket_conexion);
	}
	t_cpu * cpu = (t_cpu *) list_find(lista_cpus, (void*) es_cpu);
	int id_programa = cpu->pid;

	log_info(log_umc,
			"Solicitud de lectura de PID:%d PAGINA:%d OFFSET:%d TAMANIO:%d",
			id_programa, pagina->pagina, pagina->offset, pagina->tamanio);

	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == id_programa);
	}

	t_tabla_cantidad_entradas * cant_entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);

	//Me fijo que no se pase de la cantidad de paginas del proceso
	if (pagina->pagina > (cant_entradas->cant_paginas - 1)) {
		t_header *header_cpu = malloc(sizeof(t_header));
		header_cpu->id_proceso_emisor = PROCESO_UMC;
		header_cpu->id_proceso_receptor = PROCESO_CPU;
		header_cpu->id_mensaje = ERROR_LEER_PAGINA;
		header_cpu->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, header_cpu) < sizeof(header_cpu)) {
			log_error(log_umc, "Error de comunicacion con CPU");
		}
		free(header_cpu);
		//
	} else {

		int marco = 0;
		if (configuracion->entradas_tlb != 0) {
			log_info(log_umc, "Accediendo a la caché TLB ...");
			marco = buscar_pagina_tlb(id_programa, pagina->pagina);
			if (marco != 0) {
				log_info(log_umc,
						"Pagina encontrada en la caché TLB. Marco: %d", marco);
			}
		}

		//1° caso: esta en TLB
		if (marco) { //al ser mayor a cero quiere decir que esta en la tlb
			t_pagina_pedido_completa *pagina_cpu = malloc(
					sizeof(t_pagina_pedido_completa));
			inicializar_pagina_cpu(pagina_cpu, pagina, socket_conexion);

			t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
					lista_tablas, id_programa);
			tabla[pagina->pagina].uso = 1;

			int direccion_mp = retornar_direccion_mp(marco);

			pagina_cpu->valor = malloc(pagina->tamanio);
			memcpy(pagina_cpu->valor, (void*) direccion_mp + pagina->offset,
					pagina->tamanio);

			enviar_pagina(socket_conexion, PROCESO_CPU, pagina_cpu,
			RESPUESTA_LEER_PAGINA);


			free(pagina_cpu->valor);
			free(pagina_cpu);
			free(pagina);
			//2° caso: esta en Memoria Principal
		} else {
			log_info(log_umc,
					"Pagina no encontrada en la caché TLB. Accediendo a la Memoria Principal......");
			usleep(configuracion->retardo * 1000);
			log_info(log_umc, "Se accede a MP. Tiempo de acceso %d ms",
					configuracion->retardo);

			t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
					lista_tablas, id_programa);
			if (tabla[pagina->pagina].presencia) {

				t_pagina_pedido_completa *pagina_cpu = malloc(
						sizeof(t_pagina_pedido_completa));
				inicializar_pagina_cpu(pagina_cpu, pagina, socket_conexion);

				pagina_cpu->valor = malloc(pagina->tamanio);

				tabla[pagina->pagina].uso = 1;
				int direccion_mp = retornar_direccion_mp(
						tabla[pagina->pagina].frame);
				log_info(log_umc, "Pagina encontrada en Memoria. Marco: %d",
						tabla[pagina->pagina].frame);


				memcpy(pagina_cpu->valor, (void*) direccion_mp + pagina->offset,
						pagina->tamanio);

				if (configuracion->entradas_tlb != 0) { //valido si esta habilitada
					guardar_en_TLB(pagina_cpu->pagina, id_programa,
							tabla[pagina->pagina].frame); //pongo la pagina en la cache TLB
				}

				enviar_pagina(socket_conexion, PROCESO_CPU, pagina_cpu,
				RESPUESTA_LEER_PAGINA);

				free(pagina_cpu->valor);
				free(pagina_cpu);
				free(pagina);

			}
			//3° caso: esta en Swap
			else {
				// Pido la pagina a Swap
				log_info(log_umc,
						"Pagina no encontrada en Memoria Principal. Se procede a solicitar la pagina a SWAP");

				//me guardo el pedido
				buffer_programas[id_programa] = pagina;

				t_header *header_swap = malloc(sizeof(t_header));
				header_swap->id_proceso_emisor = PROCESO_UMC;
				header_swap->id_proceso_receptor = PROCESO_SWAP;
				header_swap->id_mensaje = MENSAJE_LEER_PAGINA;

				t_pagina *pagina_swap = malloc(sizeof(t_pagina));
				pagina_swap->id_programa = id_programa;
				pagina_swap->pagina = pagina->pagina;
				pagina_swap->offset = 0;
				pagina_swap->tamanio = configuracion->marco_size;
				pagina_swap->socket_pedido = socket_conexion;
				log_info(log_umc,
						"mando a swap a leer PID:%d PAGINA:%d OFFSET:%d TAMANIO:%d ",
						pagina_swap->id_programa, pagina_swap->pagina,
						pagina_swap->offset, pagina_swap->tamanio);
				t_buffer *payload_swap = serializar_pagina(pagina_swap);

				header_swap->longitud_mensaje = payload_swap->longitud_buffer;

				if (enviar_buffer(socket_swap, header_swap, payload_swap)
						< sizeof(t_header) + payload_swap->longitud_buffer) {
					log_error(log_umc,
							"Error de comunicacion al enviar buffer Leer pagina a SWAP - PID:%d",
							id_programa);
				}

				free(header_swap);
				free(pagina_swap);
				free(payload_swap);
			}
		}
	}
}

void respuesta_leer_pagina(void *buffer, int id_mensaje) {
	int obtuvo_marcos = 1;

	if (id_mensaje == RESPUESTA_LEER_PAGINA) {
		t_pagina_completa *pagina = malloc(sizeof(t_pagina_completa));
		deserializar_pagina_completa(buffer, pagina);

		t_pagina_pedido * pagina_pedido = buffer_programas[pagina->id_programa];

		log_info(log_umc,
				"Se recibe del SWAP la respuesta de lectura de PID:%d PAGINA:%d",
				pagina->id_programa, pagina->pagina);

		t_pagina_pedido_completa *pagina_cpu = malloc(
				sizeof(t_pagina_pedido_completa));

		pagina_cpu->pagina = pagina->pagina;
		pagina_cpu->offset = pagina_pedido->offset;
		pagina_cpu->tamanio = pagina_pedido->tamanio;
		pagina_cpu->valor = malloc(pagina_cpu->tamanio);

		t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
				lista_tablas, pagina->id_programa);

		if (guardar_en_mp(pagina) == 0) { // guardo la pagina en memoria
			log_error(log_umc,
					"Se produce un error al guardar la pagina en memoria PID:%d PAGINA:%d. No hay marcos disponibles",
					pagina->id_programa, pagina->pagina);
			id_mensaje = ERROR_LEER_PAGINA;
			obtuvo_marcos = 0;
		}

		if (obtuvo_marcos) {
			if (configuracion->entradas_tlb != 0) { //valido que este habilitada
				guardar_en_TLB(pagina->pagina, pagina->id_programa,
						tabla[pagina->pagina].frame); //pongo la pagina en la cache TLB
			}

			int direccion_mp = retornar_direccion_mp(
					tabla[pagina->pagina].frame);
			memcpy(pagina_cpu->valor, (void*) direccion_mp + pagina_cpu->offset,
					pagina_cpu->tamanio);
		}

		enviar_pagina(pagina->socket_pedido, PROCESO_CPU, pagina_cpu,
				id_mensaje);

		free(pagina->valor);
		free(pagina);
		free(pagina_pedido);
		free(pagina_cpu->valor);
		free(pagina_cpu);
	} else if (id_mensaje == ERROR_LEER_PAGINA) {
		t_pagina *pagina = malloc(sizeof(t_pagina));
		deserializar_pagina(buffer, pagina);

		t_pagina_pedido * pagina_pedido = buffer_programas[pagina->id_programa];

		t_header * header_cpu = malloc(sizeof(t_header));
		header_cpu->id_proceso_emisor = PROCESO_UMC;
		header_cpu->id_proceso_receptor = PROCESO_CPU;
		header_cpu->id_mensaje = id_mensaje;
		header_cpu->longitud_mensaje = PAYLOAD_VACIO;

		if (pagina->socket_pedido > 0) {
			if (enviar_header(pagina->socket_pedido, header_cpu)
					< sizeof(header_cpu)) {
				log_error(log_umc, "Error de comunicacion con CPU");
			} else {
				log_info(log_umc,
						"se mando al cpu la respuesta de lectura con error");
			}
		} else {
			log_info(log_umc, "El cpu ya no se encuentra disponible");
		}
		log_info(log_umc, "hubo un error al leer la pagina");
		free(pagina);
		free(pagina_pedido);
	}
}

void escribir_pagina(void *buffer, int socket_conexion) {
	t_pagina_pedido_completa *pagina = malloc(sizeof(t_pagina_pedido_completa));
	deserializar_pagina_pedido_completa(buffer, pagina);
	bool es_cpu(t_cpu *elemento) {
		return (elemento->socket_cpu == socket_conexion);
	}
	t_cpu * cpu = (t_cpu *) list_find(lista_cpus, (void*) es_cpu);
	int id_programa = cpu->pid;
	log_info(log_umc,
			"Solicitud de escritura de PID:%d PAGINA:%d OFFSET:%d TAMANIO:%d",
			id_programa, pagina->pagina, pagina->offset, pagina->tamanio);

	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == id_programa);
	}

	t_tabla_cantidad_entradas * cant_entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);

	if (pagina->pagina > (cant_entradas->cant_paginas - 1)) {
		t_header *header_cpu = malloc(sizeof(t_header));
		header_cpu->id_proceso_emisor = PROCESO_UMC;
		header_cpu->id_proceso_receptor = PROCESO_CPU;
		header_cpu->id_mensaje = ERROR_ESCRIBIR_PAGINA;
		header_cpu->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, header_cpu) < sizeof(header_cpu)) {
			log_error(log_umc, "Error de comunicacion con CPU");
		}
		free(header_cpu);
		//
	} else {

		int marco = 0;
		if (configuracion->entradas_tlb != 0) {
			log_info(log_umc, "Accediendo a la caché TLB ...");
			marco = buscar_pagina_tlb(id_programa, pagina->pagina);
			if (marco != 0) {
				log_info(log_umc,
						"Pagina encontrada en la caché TLB. Marco: %d", marco);
			}
		}
		t_header *header_cpu = malloc(sizeof(t_header));
		header_cpu->id_proceso_emisor = PROCESO_UMC;
		header_cpu->id_proceso_receptor = PROCESO_CPU;
		header_cpu->id_mensaje = RESPUESTA_ESCRIBIR_PAGINA;
		header_cpu->longitud_mensaje = PAYLOAD_VACIO;

		//1° caso: esta en TLB
		if (marco) { //al ser mayor a cero quiere decir que esta en la tlb
			int direccion_mp = retornar_direccion_mp(marco);
			memcpy((void*) direccion_mp + pagina->offset, (int*) pagina->valor,
					pagina->tamanio);
			marcar_modificada(id_programa, pagina->pagina);

			t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
					lista_tablas, id_programa);
			tabla[pagina->pagina].uso = 1;

			if (socket_conexion > 0) {
				if (enviar_header(socket_conexion, header_cpu)
						< sizeof(header_cpu)) {
					log_error(log_umc, "Error de comunicacion con CPU");
				} else {
					log_info(log_umc,
							"Se manda la respuesta al cpu de la escritura OK ");
				}
			} else {
				log_info(log_umc, "El cpu ya no se encuentra disponible");
			}

			free(header_cpu);
			free(pagina);
			//2° caso: esta en memoria
		} else {
			log_info(log_umc,
					"Pagina no encontrada en la caché TLB. Accediendo a la Memoria Principal......");
			usleep(configuracion->retardo * 1000);
			log_info(log_umc, "Se accede a MP. Tiempo de acceso %d ms",
					configuracion->retardo);
			t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
					lista_tablas, id_programa);
			if (tabla[pagina->pagina].presencia) {
				tabla[pagina->pagina].uso = 1;
				int direccion_mp = retornar_direccion_mp(
						tabla[pagina->pagina].frame);
				log_info(log_umc, "Pagina encontrada en Memoria. Marco: %d",
						tabla[pagina->pagina].frame);
				memcpy((void*) direccion_mp + pagina->offset,
						(int*) pagina->valor, pagina->tamanio);
				marcar_modificada(id_programa, pagina->pagina);
				if (configuracion->entradas_tlb != 0) {
					guardar_en_TLB(pagina->pagina, id_programa,
							tabla[pagina->pagina].frame); //pongo la pagina en la cache TLB
				}

				if (socket_conexion > 0) {
					if (enviar_header(socket_conexion, header_cpu)
							< sizeof(header_cpu)) {
						log_error(log_umc, "Error de comunicacion con CPU");
					} else {
						log_info(log_umc,
								"Se manda la respuesta al cpu de la escritura OK ");
					}
				} else {
					log_info(log_umc, "El cpu ya no se encuentra disponible");
				}

				free(header_cpu);
				free(pagina);
			}
			//3° caso: esta en Swap
			else {
				// Pido la pagina a Swap. La guardo en memoria y solo escribo en la memoria
				log_info(log_umc,
						"Pagina no encontrada en Memoria Principal. Se procede a solicitar la pagina a SWAP");

				buffer_programas[id_programa] = pagina; //guardo la pagina en un buffer

				t_header *header_swap = malloc(sizeof(t_header));
				header_swap->id_proceso_emisor = PROCESO_UMC;
				header_swap->id_proceso_receptor = PROCESO_SWAP;
				header_swap->id_mensaje = MENSAJE_LEER_PAGINA_PARA_ESCRIBIR;

				t_pagina *pagina_swap = malloc(sizeof(t_pagina));
				pagina_swap->id_programa = id_programa;
				pagina_swap->pagina = pagina->pagina;
				pagina_swap->offset = 0;
				pagina_swap->tamanio = configuracion->marco_size;
				pagina_swap->socket_pedido = socket_conexion; //es del cpu

				t_buffer *payload_swap = serializar_pagina(pagina_swap);

				header_swap->longitud_mensaje = payload_swap->longitud_buffer;

				if (enviar_buffer(socket_swap, header_swap, payload_swap)
						< sizeof(t_header) + payload_swap->longitud_buffer) {
					log_error(log_umc,
							"Error de comunicacion al enviar Leer pagina a SWAP - PID:%d",
							id_programa);
				}
				free(header_cpu);
				free(pagina_swap);
				free(header_swap);
			}
		}
	}
}

void respuesta_leer_pagina_para_escribir(void *buffer, int id_mensaje) {
	int pid;
	int socket = 0;
	int obtuvo_marcos = 1;
	t_header *header_cpu = malloc(sizeof(t_header));
	header_cpu->id_proceso_emisor = PROCESO_UMC;
	header_cpu->id_proceso_receptor = PROCESO_CPU;
	header_cpu->longitud_mensaje = PAYLOAD_VACIO;

	t_pagina_pedido_completa *pagina_buffer = malloc(
			sizeof(t_pagina_pedido_completa));

	if (id_mensaje == RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR) {
		t_pagina_completa *pagina_recibida_de_swap = malloc(
				sizeof(t_pagina_completa));
		deserializar_pagina_completa(buffer, pagina_recibida_de_swap);

		pid = pagina_recibida_de_swap->id_programa;
		socket = pagina_recibida_de_swap->socket_pedido;

		pagina_buffer = buffer_programas[pid];
		pagina_recibida_de_swap->offset = pagina_buffer->offset;
		pagina_recibida_de_swap->tamanio = pagina_buffer->tamanio;

		//Primero guardo en memoria la pagina como esta en swap y luego la sobreescribo con lo que manda la cpu
		log_info(log_umc, "Se recibe del SWAP la Pagina:%d PID:%d",
				pagina_recibida_de_swap->pagina,
				pagina_recibida_de_swap->id_programa);
		if (guardar_en_mp(pagina_recibida_de_swap) == 0) { // guardo la pagina en memoria
			header_cpu->id_mensaje = ERROR_ESCRIBIR_PAGINA;
			obtuvo_marcos = 0;
		}
		if (obtuvo_marcos) {
			t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
					lista_tablas, pagina_recibida_de_swap->id_programa);

			tabla[pagina_recibida_de_swap->pagina].presencia = 1;
			if (configuracion->entradas_tlb != 0) {
				guardar_en_TLB(pagina_recibida_de_swap->pagina,
						pagina_recibida_de_swap->id_programa,
						tabla[pagina_recibida_de_swap->pagina].frame); //pongo la pagina en la cache TLB
			}
			int direccion_mp = retornar_direccion_mp(
					tabla[pagina_recibida_de_swap->pagina].frame);

			memcpy((void*) direccion_mp + pagina_recibida_de_swap->offset,
					pagina_buffer->valor, pagina_recibida_de_swap->tamanio);

			tabla[pagina_recibida_de_swap->pagina].modificado = 1;
			header_cpu->id_mensaje = RESPUESTA_ESCRIBIR_PAGINA;
		}

		free(pagina_recibida_de_swap->valor);
		free(pagina_recibida_de_swap);
	} else if (id_mensaje == ERROR_LEER_PAGINA_PARA_ESCRIBIR) {
		log_info(log_umc,
				"Error al leer la pagina del swap, necesaria para escribir");
		t_pagina *pagina_recibida_de_swap = malloc(sizeof(t_pagina));
		deserializar_pagina(buffer, pagina_recibida_de_swap);

		pid = pagina_recibida_de_swap->id_programa;
		pagina_buffer = buffer_programas[pid];

		socket = pagina_recibida_de_swap->socket_pedido;
		log_error(log_umc,
				"Hubo un error al recibir del swap la PAGINA:%d PID:%d",
				pagina_recibida_de_swap->pagina,
				pagina_recibida_de_swap->id_programa);
		header_cpu->id_mensaje = ERROR_ESCRIBIR_PAGINA;
		free(pagina_recibida_de_swap);

	}

	if (socket > 0) {
		if (enviar_header(socket, header_cpu) < sizeof(header_cpu)) {
			log_error(log_umc, "Error de comunicacion con CPU");
		} else {
			log_info(log_umc, "Se manda la respuesta al cpu de la escritura");
		}
	} else {
		log_info(log_umc, "El cpu ya no se encuentra disponible");
	}
	free(pagina_buffer);
	free(header_cpu);
}

void respuesta_escribir_pagina(void *buffer, int id_mensaje) { //por reemplazo
	t_pagina_completa *pagina_recibida_de_swap = malloc(
			sizeof(t_pagina_completa));
	deserializar_pagina_completa(buffer, pagina_recibida_de_swap);
	if (id_mensaje == RESPUESTA_ESCRIBIR_PAGINA) { // para cuando pedi escribir por reemplazo
		log_info(log_umc,
				"ESCRITURA SATISFACTORIA EN SWAP - PID:%d - PAGINA:%d",
				pagina_recibida_de_swap->id_programa,
				pagina_recibida_de_swap->pagina);
	} else if (id_mensaje == ERROR_ESCRIBIR_PAGINA) { // el error se produjo al escribir en swap la pagina a reemplazar => no pude escribir la pagina que pidio el cpu en memoria
		log_error(log_umc, "ERROR DE ESCRITURA EN SWAP - PID:%d - PAGINA:%d",
				pagina_recibida_de_swap->id_programa,
				pagina_recibida_de_swap->pagina);
		t_header * header_cpu = malloc(sizeof(t_header));
		header_cpu->id_proceso_emisor = PROCESO_UMC;
		header_cpu->id_proceso_receptor = PROCESO_CPU;
		header_cpu->id_mensaje = ERROR_ESCRIBIR_PAGINA;
		header_cpu->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(pagina_recibida_de_swap->socket_pedido, header_cpu)
				< sizeof(header_cpu)) {
			log_error(log_umc, "Error al enviar header a cpu");
		}
	}
}

void respuesta_escribir_pagina_nueva(void *buffer, int id_mensaje) {
	t_pagina_completa *pagina_completa = malloc(sizeof(t_pagina_completa));
	deserializar_pagina_completa(buffer, pagina_completa);

	t_programa * programa_id = malloc(sizeof(t_programa));
	programa_id->id_programa = pagina_completa->id_programa;

	t_buffer * payload_nucleo = serializar_programa(programa_id);

	t_header * header_nucleo = malloc(sizeof(t_header));
	header_nucleo->id_proceso_emisor = PROCESO_UMC;
	header_nucleo->id_proceso_receptor = PROCESO_NUCLEO;
	header_nucleo->longitud_mensaje = payload_nucleo->longitud_buffer;

	if (id_mensaje == RESPUESTA_ESCRIBIR_PAGINA_NUEVA) {
		log_info(log_umc,
				"El SWAP almaceno el nuevo programa PID:%d satisfactoriamente",
				programa_id->id_programa);
		header_nucleo->id_mensaje = RESPUESTA_INICIALIZAR_PROGRAMA;
	} else if (id_mensaje == ERROR_ESCRIBIR_PAGINA_NUEVA) {
		log_error(log_umc,
				"El SWAP fallo al almacenar el nuevo programa PID:%d",
				programa_id->id_programa);
		header_nucleo->id_mensaje = ERROR_INICIALIZAR_PROGRAMA;
	}

	if (enviar_buffer(socket_nucleo, header_nucleo, payload_nucleo)
			< sizeof(t_header) + payload_nucleo->longitud_buffer) {
		log_error(log_umc, "Error de comunicacion con el Nucleo");
	}

	free(pagina_completa->valor);
	free(pagina_completa);
	free(programa_id);
	free(payload_nucleo);
	free(header_nucleo);
}

void enviar_pagina(int socket, int proceso_receptor,
		t_pagina_pedido_completa *pagina, int id_mensaje) {

	t_header *header_cpu = malloc(sizeof(t_header));
	header_cpu->id_proceso_emisor = PROCESO_UMC;
	header_cpu->id_proceso_receptor = proceso_receptor;
	header_cpu->id_mensaje = id_mensaje;

	t_buffer *payload_cpu = serializar_pagina_pedido_completa(pagina);

	header_cpu->longitud_mensaje = payload_cpu->longitud_buffer;

	if (socket > 0) {
		if (enviar_buffer(socket, header_cpu, payload_cpu)
				< sizeof(t_header) + payload_cpu->longitud_buffer) {
			log_error(log_umc, "Fallo al responder pedido CPU");
		} else {
			log_info(log_umc, "se mando al cpu la lectura solicitada");
		}
	} else {
		log_info(log_umc, "El cpu ya no se encuentra disponible");
	}

	free(header_cpu);
	free(payload_cpu);
}

void finalizar_programa(void *buffer, int id_mensaje) {
	t_programa *programa = malloc(sizeof(t_programa));
	deserializar_programa(buffer, programa);

	t_id_mensaje * mensaje = malloc(sizeof(t_id_mensaje));
	mensaje->id_mensaje = id_mensaje;

	buffer_programas[programa->id_programa] = mensaje;

	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			programa->id_programa);
	bool es_pid(t_programa * programa_a_finalizar) {
		return (programa_a_finalizar->id_programa == programa->id_programa);
	}
	t_programa * programa_a_matar = list_remove_by_condition(
			lista_procesos_activos, (void*) es_pid);

	//saco de tlb paginas asociadas al proceso
	flush_programa_tlb(programa->id_programa);

	//libero los marcos usados por ese proceso
	liberar_marcos(programa->id_programa);

	//elimino la lista usada para los algoritmos
	list_destroy(listas_algoritmo[programa->id_programa].lista_paginas_mp);

	liberar_de_listas(programa->id_programa);

	t_header *header_swap = malloc(sizeof(t_header));
	header_swap->id_proceso_emisor = PROCESO_UMC;
	header_swap->id_proceso_receptor = PROCESO_SWAP;
	header_swap->id_mensaje = MENSAJE_FINALIZAR_PROGRAMA;

	t_programa *programa_swap = malloc(sizeof(t_programa));
	programa_swap->id_programa = programa->id_programa;

	t_buffer *payload_swap = serializar_programa(programa_swap);

	header_swap->longitud_mensaje = payload_swap->longitud_buffer;

	if (enviar_buffer(socket_swap, header_swap, payload_swap)
			< sizeof(header_swap) + payload_swap->longitud_buffer) {
		log_error(log_umc, "Fallo al finalizar el programa");
	}
	log_info(log_umc, "Se mando al swap a eliminar el PROCESO:%d",
			programa_swap->id_programa);

	free(programa_a_matar);
	free(programa);
	free(header_swap);
	free(programa_swap);
	free(payload_swap);
	free(tabla);
}

void respuesta_finalizar_programa(void *buffer, int id_mensaje) {

	t_programa *programa = malloc(sizeof(t_programa));
	deserializar_programa(buffer, programa);
	log_info(log_umc, "El swap finalizo el PROCESO: %d", programa->id_programa);

	t_header *header_nucleo = malloc(sizeof(t_header));
	header_nucleo->id_proceso_emisor = PROCESO_UMC;
	header_nucleo->id_proceso_receptor = PROCESO_NUCLEO;

	t_id_mensaje *mensaje = buffer_programas[programa->id_programa];
	if (mensaje->id_mensaje == MENSAJE_MATAR_PROGRAMA) {
		header_nucleo->id_mensaje = RESPUESTA_MATAR_PROGRAMA;
	} else if (mensaje->id_mensaje == MENSAJE_FINALIZAR_PROGRAMA) {
		header_nucleo->id_mensaje = RESPUESTA_FINALIZAR_PROGRAMA;
	}

	t_buffer *payload_nucleo = serializar_programa(programa);

	header_nucleo->longitud_mensaje = payload_nucleo->longitud_buffer;
	log_info(log_umc,
			"Se manda al nucleo la confirmacion de la eliminacion del programa");
	if (enviar_buffer(socket_nucleo, header_nucleo, payload_nucleo)
			< sizeof(t_header) + payload_nucleo->longitud_buffer) {
		log_error(log_umc, "Fallo al responder pedido CPU");
	}
	free(programa);
	free(header_nucleo);
	free(payload_nucleo);
	free(mensaje);
}

void handshake_umc_cpu(int socket_cpu, t_config_umc *configuracion) {
	handshake_proceso(socket_cpu, configuracion, PROCESO_CPU,
	REPUESTA_HANDSHAKE);
	bool es_cpu(t_cpu *elemento) {
		return (elemento->socket_cpu == socket_cpu);
	}
	t_cpu * cpu = (t_cpu *) list_find(lista_cpus, (void*) es_cpu);
	if (cpu != NULL) { // hago esto por las dudas de que tenga un mismo fd que otra anterior
		pthread_mutex_lock(&mutex_lista_cpus);
		list_remove_by_condition(lista_cpus, (void*) es_cpu);
		pthread_mutex_unlock(&mutex_lista_cpus);
		free(cpu);
	}
	t_cpu * cpu_nueva = malloc(sizeof(t_cpu));
	cpu_nueva->socket_cpu = socket_cpu;
	pthread_mutex_lock(&mutex_lista_cpus);
	list_add(lista_cpus, cpu_nueva);
	pthread_mutex_unlock(&mutex_lista_cpus);
}

void handshake_umc_nucleo(int socket_conexion, t_config_umc *configuracion) {
	socket_nucleo = socket_conexion;
	handshake_proceso(socket_conexion, configuracion, PROCESO_NUCLEO,
	REPUESTA_HANDSHAKE);
}

void handshake_proceso(int socket, t_config_umc *configuracion,
		int proceso_receptor, int id_mensaje) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_UMC;
	header->id_proceso_receptor = proceso_receptor;
	header->id_mensaje = id_mensaje;

	t_pagina_tamanio *pagina = malloc(sizeof(t_pagina_tamanio));
	pagina->tamanio = configuracion->marco_size;

	t_buffer *payload = serializar_pagina_tamanio(pagina);

	header->longitud_mensaje = payload->longitud_buffer;

	if (enviar_buffer(socket, header, payload)
			< sizeof(t_header) + payload->longitud_buffer) {
		log_error(log_umc, "Fallo al enviar el tamanio de pagina");
	}

	free(header);
	free(pagina);
	free(payload);
}

void cambiar_retardo() {
	int comando_retardo;
	printf("Ingrese el retardo (en seg): ");
	scanf("%d", &comando_retardo);
	configuracion->retardo = comando_retardo;
	printf("\n");
	log_info(log_umc, "Retardo modificaco. Nuevo retardo: %d seg",
			comando_retardo);
	menu_principal(configuracion);
}

void generar_dump(int pid) {
	bool es_pid(t_programa * programa_a_finalizar) {
		return (programa_a_finalizar->id_programa == pid);
	}

	if (pid == 0) { // se pidio de todos los procesos
		pid = 1;
		while (pid < CANT_TABLAS_MAX) {
			if (list_any_satisfy(lista_procesos_activos, (void*) es_pid)) { // si es 0 no esta ese proceso activo
				dump_tabla(pid);
				dump_contenido(pid);
			}
			pid++;
		}
	} else { // se pidio de un proceso
		if (list_any_satisfy(lista_procesos_activos, (void*) es_pid)) {
			dump_tabla(pid);
			dump_contenido(pid);
		}
	}
}

//Busca una pagina dentro de la TLB. Si está retorna el marco asociado o si no está retorna null => Los marcos tendrian que empezar desde el 1
int buscar_pagina_tlb(int id_programa, int pagina) {
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	int marco;
	t_tlb * pagina_tlb;
	bool esta_en_tlb(t_tlb *un_tlb) {
		return (un_tlb->pid == id_programa && un_tlb->pagina == pagina);
	}

	pagina_tlb = list_remove_by_condition(lista_paginas_tlb,
			(void*) esta_en_tlb); //si esta se elimina para luego agregarla al final

	if (pagina_tlb == NULL) {
		marco = 0;
	} else {
		marco = pagina_tlb->frame;
		//la agrego al final
		list_add(lista_paginas_tlb, pagina_tlb);
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
	return marco;

}

int reemplazar_pagina(t_fila_tabla_pagina * pagina_a_ubicar) {
	t_fila_tabla_pagina * pagina_a_sustituir = NULL;
	int pid = pagina_a_ubicar->pid;

	//CLOCK

	if (string_equals_ignore_case(configuracion->algoritmo, "CLOCK")) {
		while (pagina_a_sustituir == NULL) {
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0) {
				break;
			} else {
				pagina_a_sustituir->uso = 0;
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
			}
		}
		quitar_pagina_TLB(pagina_a_sustituir->frame);
		pagina_a_ubicar->frame = pagina_a_sustituir->frame;
		pagina_a_ubicar->uso = 1;
		pagina_a_ubicar->modificado = 0;

		if (pagina_a_sustituir->modificado) {
			mandar_a_swap(pagina_a_sustituir->pid,
					pagina_a_sustituir->numero_pagina, MENSAJE_ESCRIBIR_PAGINA);
			pagina_a_sustituir->modificado = 0;
		}
		pagina_a_sustituir->frame = 0;
		pagina_a_sustituir->presencia = 0;
		list_replace(listas_algoritmo[pid].lista_paginas_mp,
				listas_algoritmo[pid].puntero, pagina_a_ubicar);
		listas_algoritmo[pid].puntero++;
		if (listas_algoritmo[pid].puntero
				== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
			listas_algoritmo[pid].puntero = 0;
		}
		return pagina_a_ubicar->frame;
	}

	//CLOCK MODIFICADO
	if (string_equals_ignore_case(configuracion->algoritmo, "CLOCK-M")) {

		// 1° caso: U = 0 y M = 0
		int i = 0;
		while (pagina_a_sustituir == NULL
				&& i < list_size(listas_algoritmo[pid].lista_paginas_mp)) { // hago maximo una vuelta
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0
					&& pagina_a_sustituir->modificado == 0) {
				break;
			}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
				i++;
			}
		}

		// 2° caso: U = 0 y M = 1  .  Si hay U = 1 lo pongo en U = 0
		i = 0;
		while (pagina_a_sustituir == NULL
				&& i < list_size(listas_algoritmo[pid].lista_paginas_mp)) { // hago maximo una vuelta
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0
					&& pagina_a_sustituir->modificado == 1) {
				break;
			}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir->uso = 0;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
				i++;
			}
		}

		// 3° caso: U = 1 y M = 0 ( En realidad al cambiarlo en el anterior check busco nuevamente U = 0 y M = 0)
		i = 0;
		while (pagina_a_sustituir == NULL
				&& i < list_size(listas_algoritmo[pid].lista_paginas_mp)) { // hago maximo una vuelta
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0
					&& pagina_a_sustituir->modificado == 0) {
				break;
			}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
				i++;
			}
		}

		//4° caso: U = 1 y M = 1  ( busco U = 0 y M = 1)
		while (pagina_a_sustituir == NULL) { // hago toda una vuelta
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0
					&& pagina_a_sustituir->modificado == 1) {
				break;
			}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
			}
		}
		quitar_pagina_TLB(pagina_a_sustituir->frame);
		pagina_a_ubicar->frame = pagina_a_sustituir->frame;
		pagina_a_ubicar->uso = 1;
		pagina_a_ubicar->modificado = 0;
		if (pagina_a_sustituir->modificado) {
			mandar_a_swap(pagina_a_sustituir->pid,
					pagina_a_sustituir->numero_pagina, MENSAJE_ESCRIBIR_PAGINA);
			pagina_a_sustituir->modificado = 0;
		}
		pagina_a_sustituir->frame = 0;
		pagina_a_sustituir->presencia = 0;
		list_replace(listas_algoritmo[pid].lista_paginas_mp,
				listas_algoritmo[pid].puntero, pagina_a_ubicar);
		listas_algoritmo[pid].puntero++;
		if (listas_algoritmo[pid].puntero
				== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
			listas_algoritmo[pid].puntero = 0;
		}
		return pagina_a_ubicar->frame;
	}
	//Por si no machea el nombre, que use por defecto el CLOCK
	else {
		while (pagina_a_sustituir == NULL) {
			pagina_a_sustituir = (t_fila_tabla_pagina *) list_get(
					listas_algoritmo[pid].lista_paginas_mp,
					listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0) {
				break;
			} else {
				pagina_a_sustituir->uso = 0;
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero
						== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
				}
			}
		}
		quitar_pagina_TLB(pagina_a_sustituir->frame);
		pagina_a_ubicar->frame = pagina_a_sustituir->frame;
		pagina_a_ubicar->uso = 1;
		pagina_a_ubicar->modificado = 0;
		pagina_a_sustituir->frame = 0;
		pagina_a_sustituir->presencia = 0;
		if (pagina_a_sustituir->modificado) {
			mandar_a_swap(pagina_a_sustituir->pid,
					pagina_a_sustituir->numero_pagina, MENSAJE_ESCRIBIR_PAGINA);
			pagina_a_sustituir->modificado = 0;
		}
		list_replace(listas_algoritmo[pid].lista_paginas_mp,
				listas_algoritmo[pid].puntero, pagina_a_ubicar);
		listas_algoritmo[pid].puntero++;
		if (listas_algoritmo[pid].puntero
				== list_size(listas_algoritmo[pid].lista_paginas_mp)) {
			listas_algoritmo[pid].puntero = 0;
		}
		return pagina_a_ubicar->frame;
	}
}

void quitar_pagina_TLB(int marco) {
	bool esta_en_tlb(t_tlb *un_tlb) {
		return (un_tlb->frame == marco);
	}
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	t_tlb * pagina_tlb = list_remove_by_condition(lista_paginas_tlb,
			(void*) esta_en_tlb);
	if(pagina_tlb != NULL){
		log_info(log_umc,"Se saca de la TLB - PID:%d  PAGINA:%d  MARCO:%d",pagina_tlb->pid,pagina_tlb->pagina,pagina_tlb->frame);
		free(pagina_tlb);
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
}

void guardar_en_TLB(int nro_pagina, int pid, int marco) {
	//armo la pagina tlb
	t_tlb * pagina_tlb = malloc(sizeof(t_tlb));
	pagina_tlb->frame = marco;
	pagina_tlb->pagina = nro_pagina;
	pagina_tlb->pid = pid;
	LRU(pagina_tlb);
	log_info(log_umc, "Guardo en TLB - PID:%d  PAGINA:%d  MARCO:%d", pid,
			nro_pagina, marco);
}

void LRU(t_tlb * pagina_a_ubicar) {
	//valido si hay lugar y sino reemplazo:
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	if (list_size(lista_paginas_tlb) < configuracion->entradas_tlb) {
		log_info(log_umc,"Hay lugar en la TLB");
		list_add(lista_paginas_tlb, pagina_a_ubicar);
	} else {
		t_tlb * pagina_tlb_removida = list_remove(lista_paginas_tlb, 0); //el que esta primero va a ser el Least recently used
		log_info(log_umc,"Se saca de la TLB - PID:%d  PAGINA:%d  MARCO:%d",pagina_tlb_removida->pid,pagina_tlb_removida->pagina,pagina_tlb_removida->frame);
		list_add(lista_paginas_tlb, pagina_a_ubicar);
		free(pagina_tlb_removida);
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
}

//elimina todas las entradas de un programa en la tlb
void flush_programa_tlb(int pid) {
	int i = 0;
	bool esta_en_tlb(t_tlb *un_tlb) {
		return (un_tlb->pid == pid);
	}
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	int count = list_count_satisfying(lista_paginas_tlb, (void*) esta_en_tlb);

	while (i < count) {
		t_tlb * pagina_tlb = list_remove_by_condition(lista_paginas_tlb,
				(void*) esta_en_tlb);
		free(pagina_tlb);
		i++;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
}

void flush_tlb() {
	int i = 0;
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	int count = list_size(lista_paginas_tlb);

	while (i < count) {
		t_tlb * pagina_tlb = list_remove(lista_paginas_tlb, 0);
		log_info(log_umc, "Se elimina de TLB:  PID = %d - PAGINA = %d",
				pagina_tlb->pid, pagina_tlb->pagina);
		free(pagina_tlb);
		i++;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
}

void crear_listas() {
	int i;
	//lista_buffer_escritura = list_create();//guardo t_programa_completo y t_fila_tabla_pagina - tener en cuenta por si eso da problemas
	listas_algoritmo = malloc(CANT_TABLAS_MAX * sizeof(t_lista_algoritmo)); //Para usar los algoritmos
	lista_de_marcos = list_create();
	lista_paginas_tlb = list_create();
	lista_tablas = list_create();
	lista_tabla_entradas = list_create();
	lista_cpus = list_create();
	lista_procesos_activos = list_create();
	//lista_buffer_prog_completo = list_create();
	t_fila_tabla_pagina * tabla_pagina = malloc(sizeof(t_fila_tabla_pagina)); //meto paginas vacias para que me funcione el index por lista
	tabla_pagina[0].pid = 0;
	for (i = 0; i < CANT_TABLAS_MAX; i++) {
		list_add(lista_tablas, tabla_pagina);
	}
}

int obtener_marco() {

	bool hay_marco_libre(t_marco *un_marco) {
		return (un_marco->libre == 1);
	}

	t_marco * marco = (t_marco *) list_find(lista_de_marcos,
			(void*) hay_marco_libre);
	marco->libre = 0;
	return marco->numero_marco;
}

//retorna la cantidad de paginas del proceso pid que estan en mp
int cant_pag_x_proc(int pid) {
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	t_tabla_cantidad_entradas * cant_entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);
	int i = 0;
	int cantidad = 0;
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			pid);
	while (i < cant_entradas->cant_paginas) {
		if (tabla[i].presencia) {
			cantidad++;
		}
		i++;
	}
	return cantidad;
}

int guardar_en_mp(t_pagina_completa *pagina) {
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			pagina->id_programa);
	bool hay_marco_libre(t_marco *un_marco) {
		return (un_marco->libre == 1);
	}
	int numero_marco;
	int dir_mp;

	int paginas_en_mp = cant_pag_x_proc(pagina->id_programa);

	// Posibilidad 1: Tengo marcos libres y no llegue al limite de marcos por proceso
	if (list_any_satisfy(lista_de_marcos, (void*) hay_marco_libre)
			&& paginas_en_mp < configuracion->marco_x_proc) {
		numero_marco = obtener_marco();
		log_info(log_umc, "Le asigno a PID:%d PAGINA:%d el MARCO:%d",
				pagina->id_programa, pagina->pagina, numero_marco);
		tabla[pagina->pagina].frame = numero_marco;
		tabla[pagina->pagina].presencia = 1;
		tabla[pagina->pagina].uso = 1;
		tabla[pagina->pagina].modificado = 0;
		list_add(listas_algoritmo[pagina->id_programa].lista_paginas_mp,
				&tabla[pagina->pagina]);
		dir_mp = retornar_direccion_mp(numero_marco);
		memcpy((void*) dir_mp, pagina->valor, configuracion->marco_size);
		return 1;
	}

	// Posibilidad 2: Tengo marcos libres y llegue al limite de marcos por proceso

	else if (list_any_satisfy(lista_de_marcos, (void*) hay_marco_libre)
			&& paginas_en_mp == configuracion->marco_x_proc) {
		numero_marco = reemplazar_pagina(&tabla[pagina->pagina]);
		log_info(log_umc, "Le asigno a PID:%d PAGINA:%d el MARCO:%d",
				pagina->id_programa, pagina->pagina, numero_marco);
		tabla[pagina->pagina].frame = numero_marco;
		tabla[pagina->pagina].presencia = 1;
		dir_mp = retornar_direccion_mp(numero_marco);
		memcpy((void*) dir_mp, pagina->valor, configuracion->marco_size);
		return 1;
	}
	// Posibilidad 3: No tengo marcos libres y llegue al limite de marcos por procesos
	else if (!list_any_satisfy(lista_de_marcos, (void*) hay_marco_libre)
			&& paginas_en_mp == configuracion->marco_x_proc) {
		numero_marco = reemplazar_pagina(&tabla[pagina->pagina]);
		log_info(log_umc, "Le asigno a PID:%d PAGINA:%d el MARCO:%d",
				pagina->id_programa, pagina->pagina, numero_marco);
		tabla[pagina->pagina].frame = numero_marco;
		tabla[pagina->pagina].presencia = 1;
		dir_mp = retornar_direccion_mp(numero_marco);
		memcpy((void*) dir_mp, pagina->valor, configuracion->marco_size);
		return 1;
	}
	// Posibilidad 4a: No tengo marcos libres y no llegue al limite de marcos por proceso ( ninguna esta en mp)
	// Posibilidad 4b: No tengo marcos libres y no llegue al limite de marcos por proceso ( x lo menos una esta en mp)
	else if (!(list_any_satisfy(lista_de_marcos, (void*) hay_marco_libre))
			&& paginas_en_mp < configuracion->marco_x_proc) {
		if (paginas_en_mp == 0) {
			return 0;
			log_error(log_umc,
					"No hay marcos libres y ninguna pagina del proceso esta en mp para poder ser reemplazada");
		} else {
			log_info(log_umc,
					"No hay marcos libres pero el proceso tiene paginas para ser reemplazadas");
			numero_marco = reemplazar_pagina(&tabla[pagina->pagina]);
			log_info(log_umc, "Le asigno a PID:%d PAGINA:%d el MARCO:%d",
					pagina->id_programa, pagina->pagina, numero_marco);
			tabla[pagina->pagina].frame = numero_marco;
			tabla[pagina->pagina].presencia = 1;
			dir_mp = retornar_direccion_mp(numero_marco);
			memcpy((void*) dir_mp, pagina->valor, configuracion->marco_size);
			return 1;
		}
	} else {
		log_info(log_umc, "No ocurrio nada de lo anterior");
		return 0;
	}

}

t_marco * crear_marco(int start, int nro_marco) {
	t_marco * nuevo_marco = malloc(sizeof(t_marco));
	nuevo_marco->libre = 1;
	nuevo_marco->numero_marco = nro_marco;
	nuevo_marco->direccion_mp = (int) memoria_principal + start; // direccion relativa
	return nuevo_marco;
}

void crear_marcos() {
	int i = 1;
	int start = 0;
	t_marco * nuevo_marco;
	for (i = 1; i <= configuracion->marcos; i++) { // creo los marcos
		nuevo_marco = crear_marco(start, i);
		start = start + configuracion->marco_size;
		list_add(lista_de_marcos, nuevo_marco); // numero_marco = index_de_lista + 1
	}
}

int retornar_direccion_mp(int un_marco) {
	bool es_marco(t_marco * marco) {
		return (marco->numero_marco == un_marco);
	}
	t_marco * marco_buscado = (t_marco *) list_find(lista_de_marcos,
			(void*) es_marco);
	return marco_buscado->direccion_mp;
}

void inicializar_pagina_cpu(t_pagina_pedido_completa * pagina_cpu,
		t_pagina_pedido * una_pagina, int socket_conexion) {
	pagina_cpu->pagina = una_pagina->pagina;
	pagina_cpu->offset = una_pagina->offset;
	pagina_cpu->tamanio = una_pagina->tamanio;
}

void inicializar_pagina_completa_cpu(t_pagina_completa * pagina_cpu,
		t_pagina_completa * una_pagina, int socket_conexion) {
	pagina_cpu->id_programa = una_pagina->id_programa;
	pagina_cpu->pagina = una_pagina->pagina;
	pagina_cpu->offset = una_pagina->offset;
	pagina_cpu->tamanio = una_pagina->tamanio;
	pagina_cpu->socket_pedido = socket_conexion;
}

void marcar_todas_modificadas() {
	int pid = 1;
	int nro_pagina = 0;
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	bool es_pid(t_programa * programa_a_finalizar) {
		return (programa_a_finalizar->id_programa == pid);
	}
	t_tabla_cantidad_entradas * entradas;
	while (pid < CANT_TABLAS_MAX) {
		if (list_any_satisfy(lista_procesos_activos, (void*) es_pid)) { // si es 0 no esta ese proceso activo
			entradas = (t_tabla_cantidad_entradas *) list_find(
					lista_tabla_entradas, (void*) es_true);
			while (nro_pagina < entradas->cant_paginas) {
				marcar_modificada(pid, nro_pagina);
				nro_pagina++;
			}
			nro_pagina = 0;
		}
		pid++;
	}
}

void marcar_modificada(int pid, int pagina) {
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			pid);
	if (tabla[pagina].presencia) {
		tabla[pagina].modificado = 1;
	}
}

void mandar_a_swap(int pid, int pagina, int id_mensaje) {
	t_header *header_swap = malloc(sizeof(t_header));
	header_swap->id_proceso_emisor = PROCESO_UMC;
	header_swap->id_proceso_receptor = PROCESO_SWAP;
	header_swap->id_mensaje = id_mensaje;

	t_pagina_completa * pagina_completa = malloc(sizeof(t_pagina_completa));
	pagina_completa->id_programa = pid;
	pagina_completa->offset = 0;
	pagina_completa->pagina = pagina;
	pagina_completa->socket_pedido = 0;

	if (id_mensaje == MENSAJE_ESCRIBIR_PAGINA_NUEVA) {
		//busco el contenido que me mando el nucleo en el buffer en que lo guarde y se lo mando al swap

		//t_programa_completo * prog_comp = malloc(sizeof(t_programa_completo));
		pagina_completa->valor = buffer_programas[pid];

		pagina_completa->tamanio = strlen(buffer_programas[pid]);

		log_info(log_umc, "Se envia el codigo del nuevo programa a swap ...");
	} else if (id_mensaje == MENSAJE_ESCRIBIR_PAGINA) {
		//busco en memoria el contenido de la pagina y se lo mando al swap
		pagina_completa->tamanio = configuracion->marco_size;
		pagina_completa->valor = malloc(pagina_completa->tamanio);
		t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(
				lista_tablas, pid);
		int dir_mp = retornar_direccion_mp(tabla[pagina].frame);
		memcpy(pagina_completa->valor, (void*) dir_mp,
				configuracion->marco_size);
	}
	t_buffer *payload_swap = serializar_pagina_completa(pagina_completa);

	header_swap->longitud_mensaje = payload_swap->longitud_buffer;

	if (enviar_buffer(socket_swap, header_swap, payload_swap)
			< sizeof(t_header) + payload_swap->longitud_buffer) {
		log_error(log_umc, "Fallo al responder pedido CPU");
	}

	free(pagina_completa->valor);
	free(pagina_completa);
	free(header_swap);
	free(payload_swap);
}

void dump_tabla(int pid) {
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	t_tabla_cantidad_entradas * cant_entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);
	int nro_pagina = 0;
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			pid);
	txt_write_in_file(dump_file,
			"Estructuras de memoria:\n\nTABLA DE PAGINAS DE PROCESO: ");
	txt_write_in_stdout(
			"Estructuras de memoria:\n\nTABLA DE PAGINAS DE PROCESO: ");
	char * numero = malloc(sizeof(int));

	numero = (char*) string_itoa(pid);
	txt_write_in_file(dump_file, numero); //pid
	txt_write_in_stdout(numero);
	txt_write_in_file(dump_file,
			"\n--------------------------------------------------------\n|   PAGINA    |  PRESENCIA  | MODIFICADA  |    MARCO    |\n--------------------------------------------------------\n");
	txt_write_in_stdout(
			"\n--------------------------------------------------------\n|   PAGINA    |  PRESENCIA  | MODIFICADA  |    MARCO    |\n--------------------------------------------------------\n");
	while (nro_pagina < cant_entradas->cant_paginas) {
		char * pagina = string_new();
		string_append(&pagina, "|      ");
		numero = (char*) string_itoa(tabla[nro_pagina].numero_pagina);
		string_append(&pagina, numero); //pagina
		string_append(&pagina, "      |      ");
		numero = (char*) string_itoa(tabla[nro_pagina].presencia);
		string_append(&pagina, numero); //presencia
		string_append(&pagina, "      |      ");
		numero = (char*) string_itoa(tabla[nro_pagina].modificado);
		string_append(&pagina, numero); //modficada
		string_append(&pagina, "      |      ");
		numero = (char*) string_itoa(tabla[nro_pagina].frame);
		string_append(&pagina, numero); //marco
		string_append(&pagina, "      |");
		txt_write_in_file(dump_file, pagina);
		txt_write_in_stdout(pagina);
		txt_write_in_file(dump_file,
				"\n--------------------------------------------------------\n");
		txt_write_in_stdout(
				"\n--------------------------------------------------------\n");
		nro_pagina++;
		free(pagina);
	}
	txt_write_in_file(dump_file, "\n\n");
	txt_write_in_stdout("\n\n");
	free(numero);
}

void dump_contenido(int pid) {
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	char * numero_var = malloc(sizeof(int));
	char * pagina = malloc(configuracion->marco_size);
	t_tabla_cantidad_entradas * cant_entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);
	int nro_pagina = 0;
	int direccion_mp;
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina *) list_get(lista_tablas,
			pid);
	char * string = string_new();
	string_append(&string, "\nContenido de memoria:\n\nPROCESO: ");
	numero_var = (char*) string_itoa(pid);
	string_append(&string, numero_var);
	txt_write_in_file(dump_file, string);
	txt_write_in_stdout(string);
	char * string2;
	while (nro_pagina < cant_entradas->cant_paginas) {
		if (tabla[nro_pagina].presencia) {
			string2 = string_new();
			txt_write_in_file(dump_file, "\n");
			txt_write_in_stdout("\n");
			string_append(&string2, "Pagina: ");
			numero_var = (char*) string_itoa(tabla[nro_pagina].numero_pagina);
			string_append(&string2, numero_var);
			txt_write_in_file(dump_file, string2);
			txt_write_in_stdout(string2);
			txt_write_in_file(dump_file, "\n");
			txt_write_in_stdout("\n");
			direccion_mp = retornar_direccion_mp(tabla[nro_pagina].frame);
			memcpy(pagina, (void*) direccion_mp, configuracion->marco_size);
			txt_write_in_file_all(dump_file, pagina, pid, nro_pagina);
			txt_write_in_stdout_all(pagina, pid, nro_pagina);
			txt_write_in_file(dump_file, "\n");
			txt_write_in_stdout("\n");
			free(string2);
		}
		nro_pagina++;
	}
	txt_write_in_stdout("\n\nDUMP FINALIZADO\n\n");
	free(numero_var);
	free(string);
	free(pagina);
}

void guardar_cant_entradas(int pid, int cant_pag, int tamanio) {
	t_tabla_cantidad_entradas * tabla_cant = malloc(
			sizeof(t_tabla_cantidad_entradas));
	tabla_cant->cant_paginas = cant_pag;
	tabla_cant->pid = pid;
	tabla_cant->tamanio_codigo = tamanio;
	pthread_mutex_lock(&mutex_lista_tabla_entradas);
	list_add(lista_tabla_entradas, tabla_cant);
	pthread_mutex_unlock(&mutex_lista_tabla_entradas);
} //TODO: ver de eliminarlas al finalizar pid

void liberar_de_listas(int pid) {
	//libera de la lista de cantidad de entradas
	bool es_pid(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	pthread_mutex_lock(&mutex_lista_tabla_entradas);
	t_tabla_cantidad_entradas * tabla_cant =
			(t_tabla_cantidad_entradas *) list_remove_by_condition(
					lista_tabla_entradas, (void*) es_pid);
	pthread_mutex_unlock(&mutex_lista_tabla_entradas);
	free(tabla_cant);
}

void liberar_marcos(int pid) {
	int nro_pagina = 0;
	int un_marco;
	t_tabla_cantidad_entradas * entradas;
	t_fila_tabla_pagina * tabla = (t_fila_tabla_pagina*) list_get(lista_tablas,
			pid);
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	bool es_marco(t_marco * marco) {
		return (marco->numero_marco == un_marco);
	}
	entradas = (t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
			(void*) es_true);
	while (nro_pagina < entradas->cant_paginas) {
		if (tabla[nro_pagina].presencia) {
			un_marco = tabla[nro_pagina].frame;
			tabla[nro_pagina].frame = 0;
			t_marco * marco = (t_marco *) list_find(lista_de_marcos,
					(void*) es_marco);
			marco->libre = 1;
			log_info(log_umc, "Se libera el marco %d", marco->numero_marco);
		}
		nro_pagina++;
	}
}

void cambiar_proceso_activo(void * buffer, int socket) {
	t_programa *programa = malloc(sizeof(t_programa));
	deserializar_programa(buffer, programa);
	t_header * header_cpu = malloc(sizeof(t_header));
	header_cpu->id_proceso_emisor = PROCESO_UMC;
	header_cpu->id_proceso_receptor = PROCESO_CPU;
	header_cpu->id_mensaje = RESPUESTA_CAMBIO_PROCESO_ACTIVO;
	header_cpu->longitud_mensaje = PAYLOAD_VACIO;
	log_info(log_umc,
			"Se procede a tratar un nuevo proceso por la CPU. PID: %d",
			programa->id_programa);
	bool es_cpu(t_cpu *elemento) {
		return (elemento->socket_cpu == socket);
	}
	if (programa->id_programa == -1) {
		log_info(log_umc, "Finaliza un CPU");
		finalizar_cpu(socket);
	} else if (programa->id_programa == 0) { // ya no se usa
		t_cpu * cpu = (t_cpu *) list_find(lista_cpus, (void*) es_cpu);
		cpu->pid = programa->id_programa;
	} else {
		flush_programa_tlb(programa->id_programa);
		t_cpu * cpu = (t_cpu *) list_find(lista_cpus, (void*) es_cpu);
		cpu->pid = programa->id_programa;
	}
	if (enviar_header(socket, header_cpu) < sizeof(header_cpu)) {
		log_error(log_umc, "Error de comunicacion");
	}
	free(programa);
}

void txt_write_in_file_all(FILE* file, char* bytes, int pid, int nro_pagina) {
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	t_tabla_cantidad_entradas * entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);

	int offset_comienzo_stack = entradas->tamanio_codigo
			% configuracion->marco_size;
	int pagina_comienzo_stack = entradas->tamanio_codigo
			/ configuracion->marco_size;

	int i = 0;
	while (i < configuracion->marco_size) {
		if (bytes[i] == '\0') {
			fprintf(file, " ");
		} else {
			if (nro_pagina > pagina_comienzo_stack) { // si quiero ver los valores del stack, descomento y pongo valor=offset en que comienza el stack
				fprintf(file, "%c", bytes[i]);
			} else if (nro_pagina == pagina_comienzo_stack) {
				if (i < offset_comienzo_stack) {
					fprintf(file, "%c", bytes[i]);
				} else {
					fprintf(file, "%d", bytes[i]);
				}
			} else { //nro_pagina < pagina_comienzo_stack
				fprintf(file, "%c", bytes[i]);
			}
		}
		i++;
	}
	fflush(file);
}

void txt_write_in_stdout_all(char* string, int pid, int nro_pagina) {
	bool es_true(t_tabla_cantidad_entradas *elemento) {
		return (elemento->pid == pid);
	}
	t_tabla_cantidad_entradas * entradas =
			(t_tabla_cantidad_entradas *) list_find(lista_tabla_entradas,
					(void*) es_true);

	int offset_comienzo_stack = entradas->tamanio_codigo
			% configuracion->marco_size;
	int pagina_comienzo_stack = entradas->tamanio_codigo
			/ configuracion->marco_size;

	int i = 0;
	while (i < configuracion->marco_size) {
		if (string[i] == '\0') {
			printf(" ");
		} else {
			if (nro_pagina > pagina_comienzo_stack) { // si quiero ver los valores del stack, descomento y pongo valor=offset en que comienza el stack
				printf("%c", string[i]);
			} else if (nro_pagina == pagina_comienzo_stack) {
				if (i < offset_comienzo_stack) {
					printf("%c", string[i]);
				} else {
					printf("%d", string[i]);
				}
			} else { //nro_pagina < pagina_comienzo_stack
				printf("%c", string[i]);
			}
		}
		i++;
	}
	fflush(stdout);
}

void finalizar_cpu(int socket_cpu) {

	bool es_cpu(t_cpu *elemento) {
		return (elemento->socket_cpu == socket_cpu);
	}
	pthread_mutex_lock(&mutex_lista_cpus);
	t_cpu * cpu = list_remove_by_condition(lista_cpus, (void*) es_cpu);
	pthread_mutex_unlock(&mutex_lista_cpus);

	if (cpu != NULL) {
		flush_programa_tlb(cpu->pid);
		free(cpu);
	}
}

void test_tlb() {
	int i = 0;
	pthread_mutex_lock(&mutex_lista_paginas_tlb);
	log_info(log_umc,
			"El contenido de la tlb, desde el ultimo al mas reciente referenciado, es el siguiente:");
	log_info(log_umc, "-----------------");
	log_info(log_umc, "|  PID  | PAGINA|");
	log_info(log_umc, "-----------------");
	int cantidad = list_size(lista_paginas_tlb);
	while (i < cantidad) {
		t_tlb * un_tlb = list_get(lista_paginas_tlb, i);
		log_info(log_umc, "|   %d   |   %d   |", un_tlb->pid, un_tlb->pagina);
		log_info(log_umc, "-----------------");
		i++;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_tlb);
}
