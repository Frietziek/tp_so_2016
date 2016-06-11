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
#include <unistd.h> // Funcion sleep y close
#include <commons/config.h>
#include <comunicaciones.h>
#include <serializacion.h>
#include <commons/collections/list.h>
#include "umc.h"

int socket_swap;
int socket_nucleo;
void *memoria_principal;
void *cache_tlb;
t_lista_algoritmo * listas_algoritmo;
t_list *lista_de_marcos,*lista_paginas_tlb,*lista_tablas;
t_config_umc *configuracion;

int main(void) {
	configuracion = malloc(sizeof(t_config_umc)); // Estructura de configuracion de la UMC
	carga_configuracion_UMC("src/config.umc.ini", configuracion);

	// Se crea el bloque de la memoria principal
	memoria_principal = calloc(configuracion->marcos,
			configuracion->marco_size);
	// TODO Crear estructura para el manejo de memoria principal

	// Creo Cache TLB
	cache_tlb = calloc(configuracion->entradas_tlb, sizeof(t_tlb));

	//iniciar listas
	lista_de_marcos = list_create();
	lista_paginas_tlb = list_create();
	lista_tablas = list_create();

	// Inicio servidor UMC
	t_configuracion_servidor* servidor_umc = creo_servidor_umc(configuracion);

	// Se realiza la conexión con el swap
	//socket_swap = conecto_con_swap(configuracion);

	test();

	menu_principal(configuracion);

	free(configuracion);
	free(servidor_umc);
	close(socket_swap);
	free(memoria_principal);
	free(cache_tlb);
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
	printf("Servidor UMC corriendo\n");
	return servidor_umc;
}

int conecto_con_swap(t_config_umc* configuracion) {
	int socket_servidor;
	if ((socket_servidor = conectar_servidor(configuracion->ip_swap,
			configuracion->puerto_swap, &atender_swap)) > 0) {
		printf("UMC conectado con SWAP\n");
		handshake_umc_swap(socket_servidor, configuracion);
	} else {
		perror("Error al conectarse con el Swap");
	}
	return socket_servidor;
}

void menu_principal(t_config_umc *configuracion) {
	int comando;
	// Comandos ingresados de la consola de UMC
	printf("Ingrese uno de los siguientes comandos para continuar:\n");
	printf("1 - Cambiar retardo de la consola UMC\n");
	printf("2 - Generar reporte y archivo Dump\n");
	printf("3 - Limpiar contenido de LTB o paginas\n");
	scanf("%d", &comando);
	switch (comando) {
	case RETARDO:
		cambiar_retardo(configuracion);
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
		perror("No tiene permisos para comunicarse con la UMC\n");
		break;
	}
}

void atender_cpu(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		printf("Handshake recibido de CPU\n");
		handshake_umc_cpu(socket_conexion, configuracion);
		break;
	case MENSAJE_LEER_PAGINA:
		printf("Recibido mensaje derefenciar\n");
		leer_pagina(paquete->payload, socket_conexion, configuracion);
		break;
	case MENSAJE_ESCRIBIR_PAGINA:
		printf("Recibido mensaje asignar\n");
		escribir_pagina(paquete->payload, socket_conexion);
		break;
	case MENSAJE_CAMBIO_PROCESO_ACTIVO:
		// TODO Terminar funcion
		break;
	default:
		perror("Comando no reconocido\n");
		break;
	}
}

void atender_nucleo(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion) {
	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		printf("Handshake recibido de Nucleo\n");
		handshake_umc_nucleo(socket_conexion, configuracion);
		break;
	case MENSAJE_INICIALIZAR_PROGRAMA:
		printf("Creo nuevo programa y mando al Swap\n");
		iniciar_programa(paquete->payload);
		break;
	default:
		perror("Comando no reconocido\n");
		break;
	}
}

void atender_swap(t_paquete *paquete, int socket_conexion) {
	switch (paquete->header->id_mensaje) {
	case REPUESTA_HANDSHAKE:
		respuesta_handshake_umc_swap();
		break;
	case RESPUESTA_LEER_PAGINA:
		respuesta_leer_pagina(paquete->payload);
		break;
	case RESPUESTA_ESCRIBIR_PAGINA:
		printf("Escritura de pagina exitosa\n");
		break;
	case RESPUESTA_INICIAR_PROGRAMA:
		respuesta_iniciar_programa(paquete->payload);
		break;
	case RESPUESTA_FINALIZAR_PROGRAMA:
		// TODO Terminar funcion
		printf("Finalizacion del programa exitoso");
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

void handshake_umc_swap(int socket_servidor, t_config_umc *configuracion) {
	handshake_proceso(socket_servidor, configuracion, PROCESO_SWAP,
	MENSAJE_HANDSHAKE);
}

void respuesta_handshake_umc_swap() {
	printf("Handshake de Swap confirmado\n");
}

void iniciar_programa(void *buffer) {
	int i;
	t_programa_completo *programa = malloc(sizeof(t_programa_completo));
	deserializar_programa_completo(buffer, programa);
	t_fila_tabla_pagina * tabla_paginas = (t_fila_tabla_pagina *) malloc(programa->paginas_requeridas * sizeof(t_fila_tabla_pagina));


	//armo la tabla:
	for(i = 0; i < programa->paginas_requeridas; i++){

		tabla_paginas[i].frame = NULL;
		tabla_paginas[i].modificado = 0;
		tabla_paginas[i].pid = programa->id_programa;
		tabla_paginas[i].presencia = 0;
		tabla_paginas[i].uso = 0;
		tabla_paginas[i].numero_pagina = i;
		printf(" tabla_paginas[0]->pid = %d \n" ,tabla_paginas[i].pid);//para probar
	}

	list_add_in_index(lista_tablas,tabla_paginas,programa->id_programa); // lista de tablas. El index va a coincidir con el pid

	//tablas_de_procesos + programa->paginas_requeridas  = malloc(programa->paginas_requeridas * sizeof(t_fila_tabla_pagina));
	//tablas_de_procesos + programa->paginas_requeridas = tabla_paginas; // array con todas las tablas de pag de los diferentes procesos

	// TODO Verificar que se administren bien
	// Creo la tabla de paginas del proceso


	t_header *header_swap = malloc(sizeof(t_header));
	header_swap->id_proceso_emisor = PROCESO_UMC;
	header_swap->id_proceso_receptor = PROCESO_SWAP;
	header_swap->id_mensaje = MENSAJE_INICIAR_PROGRAMA;

	t_programa_completo *programa_swap = malloc(sizeof(t_programa_completo));
	programa_swap->id_programa = programa->id_programa;
	programa_swap->paginas_requeridas = programa->paginas_requeridas;
	programa_swap->codigo = programa->codigo;

	t_buffer *payload_swap = serializar_programa_completo(programa_swap);

	header_swap->longitud_mensaje = payload_swap->longitud_buffer;

	if (enviar_buffer(socket_swap, header_swap, payload_swap)
			< sizeof(header_swap) + payload_swap->longitud_buffer) {
		perror("Fallo al iniciar el programa");
	}

	free(programa);
	free(header_swap);
	free(programa_swap);
	free(payload_swap);

}

void respuesta_iniciar_programa(void *buffer) {

	t_programa_completo *programa = malloc(sizeof(t_programa_completo));
	deserializar_programa_completo(buffer, programa);

	t_header *header_nucleo = malloc(sizeof(t_header));
	header_nucleo->id_proceso_emisor = PROCESO_UMC;
	header_nucleo->id_proceso_receptor = PROCESO_NUCLEO;
	// TODO Definir los mensajes entre el nucleo y umc y actualizar esta linea
	header_nucleo->id_proceso_receptor = RESPUESTA_INICIALIZAR_PROGRAMA;
	header_nucleo->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_nucleo, header_nucleo) < sizeof(header_nucleo)) {
		perror("Error al iniciar programa");
	}

	// TODO Asignar paginas de la memoria princ para el programa nuevo

	free(programa);
	free(header_nucleo);

}

void leer_pagina(void *buffer, int socket_conexion, t_config_umc *configuracion) {

	t_pagina *pagina = malloc(sizeof(t_pagina));
	deserializar_pagina(buffer, pagina);

	if (pagina_en_tlb()) {
		// Devuelvo la pagina pedida
		t_pagina_completa *pagina_cpu = malloc(sizeof(t_pagina_completa));
		pagina_cpu->id_programa = pagina->id_programa;
		pagina_cpu->pagina = pagina->pagina;
		pagina_cpu->offset = pagina->offset;
		pagina_cpu->tamanio = pagina->tamanio;
		pagina_cpu->socket_pedido = socket_conexion;

		// TODO Cargar la pagina desde la memoria, completar con el valor real
		pagina_cpu->valor = malloc(pagina->tamanio);
		memcpy(pagina_cpu->valor, "variables a, b", 14);
		//pagina_cpu->valor = "variables a, b";

		enviar_pagina(socket_conexion, PROCESO_CPU, pagina_cpu);

		free(pagina_cpu);

	} else {

		sleep(configuracion->retardo);

		// Pido la pagina a Swap
		t_header *header_swap = malloc(sizeof(t_header));
		header_swap->id_proceso_emisor = PROCESO_UMC;
		header_swap->id_proceso_receptor = PROCESO_SWAP;
		header_swap->id_mensaje = MENSAJE_LEER_PAGINA;

		t_pagina *pagina_swap = malloc(sizeof(t_pagina));
		pagina_swap->pagina = pagina->pagina;
		pagina_swap->offset = pagina->offset;
		pagina_swap->tamanio = pagina->tamanio;
		pagina_swap->socket_pedido = socket_conexion;

		t_buffer *payload_swap = serializar_pagina(pagina_swap);

		header_swap->longitud_mensaje = payload_swap->longitud_buffer;

		if (enviar_buffer(socket_swap, header_swap, payload_swap)
				< sizeof(t_header) + payload_swap->longitud_buffer) {
			perror("Fallo enviar buffer Leer pagina de Swap\n");
		}

		free(header_swap);
		free(pagina_swap);
		free(payload_swap);

	}

	//free(pagina);
}

void respuesta_leer_pagina(void *buffer) {

	printf("Respuesta leer pagina\n");

	t_pagina_completa *pagina = malloc(sizeof(t_pagina_completa));
	deserializar_pagina_completa(buffer, pagina);

	// TODO Cargo la pagina a memoria, utilizando LRU

	enviar_pagina(pagina->socket_pedido, PROCESO_CPU, pagina);

	free(pagina);

}

void enviar_pagina(int socket, int proceso_receptor, t_pagina_completa *pagina) {

	t_header *header_cpu = malloc(sizeof(t_header));
	header_cpu->id_proceso_emisor = PROCESO_UMC;
	header_cpu->id_proceso_receptor = proceso_receptor;
	header_cpu->id_mensaje = RESPUESTA_LEER_PAGINA;

	t_buffer *payload_cpu = serializar_pagina_completa(pagina);

	header_cpu->longitud_mensaje = payload_cpu->longitud_buffer;

	if (enviar_buffer(socket, header_cpu, payload_cpu)
			< sizeof(t_header) + payload_cpu->longitud_buffer) {
		perror("Fallo al responder pedido CPU");
	}

	free(header_cpu);
	free(payload_cpu);

}

void escribir_pagina(void *buffer, int socket_conexion) {
	// TODO Terminar funcion
}

void finalizar_programa(void *buffer, int socket) {

	t_programa *programa = malloc(sizeof(t_programa));
	deserializar_programa(buffer, programa);

	// TODO Marcar en la tabla de paginas, las que quedaron libres

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
		perror("Fallo al finalizar el programa");
	}

	free(programa);
	free(header_swap);
	free(programa_swap);
	free(payload_swap);

}

void respuesta_finalizar_programa(void *buffer) {

	t_programa_completo *programa = malloc(sizeof(t_programa_completo));
	deserializar_programa_completo(buffer, programa);

	t_header *header_nucleo = malloc(sizeof(t_header));
	header_nucleo->id_proceso_emisor = PROCESO_SWAP;
	header_nucleo->id_proceso_receptor = PROCESO_NUCLEO;
	// TODO Definir los mensajes entre el nucleo y umc y actualizar esta linea
	header_nucleo->id_proceso_receptor = RESPUESTA_FINALIZAR_PROGRAMA;
	header_nucleo->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_nucleo, header_nucleo) < sizeof(header_nucleo)) {
		perror("Error al finalizar programa");
	}

	free(programa);
	free(header_nucleo);

}

void handshake_umc_cpu(int socket_cpu, t_config_umc *configuracion) {
	handshake_proceso(socket_cpu, configuracion, PROCESO_CPU,
	REPUESTA_HANDSHAKE);
}

void handshake_umc_nucleo(int socket_nucleo, t_config_umc *configuracion) {
	handshake_proceso(socket_nucleo, configuracion, PROCESO_NUCLEO,
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
		perror("Fallo al enviar el tamanio de pagina");
	}

	free(header);
	free(pagina);
	free(payload);
}

int pagina_en_tlb() {
	// TODO Terminar funcion
	return 1;
}

void cambiar_retardo(t_config_umc *configuracion) {
	int comando_retardo;
	printf("Ingrese el retardo (en segundos): ");
	scanf("%d", &comando_retardo);
	configuracion->retardo = comando_retardo;
	printf("\n");
	menu_principal(configuracion);
}

void generar_dump() {
// TODO Terminar funcion
}

void limpiar_contenido() {
// TODO Terminar funcion
}

//Busca una pagina dentro de la TLB. Si está retorna el marco asociado o si no está retorna null
t_marco * buscar_pagina_tlb(int id_programa,int pagina){
	t_marco * marco;
	bool esta_en_tlb(t_tlb *un_tlb){
		return (un_tlb->pid == id_programa && un_tlb->pagina == pagina);
	}
	marco = list_find(lista_paginas_tlb,(void*)esta_en_tlb);
	return marco;
}

t_marco * buscar_pagina_mp(int id_programa,int pagina){
	t_marco * marco;

	bool esta_en_memoria(t_fila_tabla_pagina *una_pagina_mp){
		return (una_pagina_mp->pid == id_programa && una_pagina_mp->numero_pagina == pagina);
	}
	marco = list_find(lista_paginas_tlb,(void*)esta_en_memoria);
	return marco;
}



void reemplazar_pagina(t_fila_tabla_pagina * pagina_a_ubicar){
	t_fila_tabla_pagina * pagina_a_sustituir = NULL;
	int pid = pagina_a_ubicar->pid;

	//CLOCK
	if(string_equals_ignore_case(configuracion->algoritmo,"CLOCK")){
		while (pagina_a_sustituir == NULL ){
			pagina_a_sustituir = list_get(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0) {
				break;
			}
			else {
				pagina_a_sustituir->uso = 0;
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero == list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
					}
			}
		}
		pagina_a_ubicar->frame = pagina_a_sustituir->frame;
		list_add_in_index(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero,pagina_a_ubicar);
		// ver que mas tengo que hacer
	}

	//CLOCK MODIFICADO
	if(string_equals_ignore_case(configuracion->algoritmo,"CLOCK_MODIFICADO")){

		// 1° caso: U = 0 y M = 0
		int i = 0;
		while(pagina_a_sustituir == NULL && i < list_size(listas_algoritmo[pid].lista_paginas_mp)){ // hago maximo una vuelta
			pagina_a_sustituir = list_get(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0 && pagina_a_sustituir->modificado == 0) {  break;}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero == list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
					}
				i++;
			}
		}

		// 2° caso: U = 0 y M = 1  .  Si hay U = 1 lo pongo en U = 0
		i = 0;
		while(pagina_a_sustituir == NULL && i < list_size(listas_algoritmo[pid].lista_paginas_mp)){ // hago maximo una vuelta
			pagina_a_sustituir = list_get(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0 && pagina_a_sustituir->modificado == 1) {  break;}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir->uso = 0;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero == list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
					}
				i++;
			}
		}

		// 3° caso: U = 1 y M = 0 ( En realidad al cambiarlo en el anterior check busco nuevamente U = 0 y M = 0)
		i = 0;
		while(pagina_a_sustituir == NULL && i < list_size(listas_algoritmo[pid].lista_paginas_mp)){ // hago maximo una vuelta
			pagina_a_sustituir = list_get(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0 && pagina_a_sustituir->modificado == 0) {  break;}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				if (listas_algoritmo[pid].puntero == list_size(listas_algoritmo[pid].lista_paginas_mp)) {
					listas_algoritmo[pid].puntero = 0;
					}
				i++;
			}
		}

		//4° caso: U = 1 y M = 1  ( busco U = 0 y M = 1)
		while(pagina_a_sustituir == NULL){ // hago toda una vuelta
			pagina_a_sustituir = list_get(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero);
			if (pagina_a_sustituir->uso == 0 && pagina_a_sustituir->modificado == 0) {  break;}  //encuentro
			else {
				listas_algoritmo[pid].puntero++;
				pagina_a_sustituir = NULL;
				i++;
			}
		}

	pagina_a_ubicar->frame = pagina_a_sustituir->frame;
	list_add_in_index(listas_algoritmo[pid].lista_paginas_mp,listas_algoritmo[pid].puntero,pagina_a_ubicar);
	// ver que mas tengo que hacer

	}
}

void test(){
	t_programa_completo *programa1 = malloc(sizeof(t_programa_completo));
	t_programa_completo *programa2 = malloc(sizeof(t_programa_completo));
	t_programa_completo *programa3 = malloc(sizeof(t_programa_completo));
	programa1->id_programa = 1;
	programa1->paginas_requeridas = 3;
	programa1->codigo = malloc(10);
	programa1->codigo = "soy pid 1\0";
	programa2->id_programa = 2;
	programa2->paginas_requeridas = 1;
	programa2->codigo = malloc(10);
	programa2->codigo = "soy pid 2\0";
	programa3->id_programa = 3;
	programa3->paginas_requeridas = 4;
	programa3->codigo = malloc(10);
	programa3->codigo = "soy pid 3\0";
	t_buffer * buffer1 = serializar_programa_completo(programa1);
	t_buffer * buffer2 = serializar_programa_completo(programa2);
	t_buffer * buffer3 = serializar_programa_completo(programa3);
	iniciar_programa(buffer1->contenido_buffer);
	iniciar_programa(buffer2->contenido_buffer);
	iniciar_programa(buffer3->contenido_buffer);
	t_fila_tabla_pagina * tabla3 =  list_get(lista_tablas, 3);
	printf("pid 2 -> tabla3[2].numero_pagina %d \n",tabla3[2].numero_pagina);
}
