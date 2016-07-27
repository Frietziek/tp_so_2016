/*
 ============================================================================
 Name        : swap.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <comunicaciones.h>
#include "tipos_swap.h"
#include "utilidades_swap.h"
#include "serializacion_swap_umc.h"
#include "funciones_swap.h"

/*---------------Variables globales---------------*/
t_log *loggerManager;
t_bitarray *paginas_bitmap;
t_list *lista_programas;
t_config_swap *config_swap;
FILE *archivo_swap;
/*------------------------------------------------*/

int main(void) {

	loggerManager = log_create("swap.log","SWAP", true, LOG_LEVEL_TRACE); //creo un archivo log


	/********************************  Configuraciones SWAP  ********************************/

	config_swap = malloc(sizeof(t_config_swap));
	config_swap->nombre_swap = malloc(30);
	cargar_configuracion_swap("config.swap.ini",config_swap);
	log_trace(loggerManager, "\nSe cargaron las configuraciones con los siguientes valores: \nPUERTO_ESCUCHA=%i \nNOMBRE_SWAP=%s\nCANTIDAD_PAGINAS=%i\nTAMANO_PAGINA=%i\nRETARDO_COMPACTACION=%i\nRETARDO_ACCESO=%i\n", config_swap->puerto_escucha, config_swap->nombre_swap, config_swap->cantidad_paginas, config_swap->tamano_pagina, config_swap->retardo_compactacion, config_swap->retardo_acceso);

	/***************************************************************************************/


	/***********************************  Archivo SWAP  ***********************************/

	int status = crear_archivo_swap(config_swap->nombre_swap, config_swap->cantidad_paginas * config_swap->tamano_pagina); //creo e inicializo el archivo SWAP
	if(status!=-1)
		log_trace(loggerManager,"Se creo correctamente el archivo swap");
	else
		log_error(loggerManager,"Hubo un problema al intentar crear el archivo swap");

	archivo_swap = fopen(config_swap->nombre_swap, "rb+");

	/**************************************************************************************/


	/******************************  Estructuras de control  ******************************/
	float tamano_paginas_array;

	if((config_swap->cantidad_paginas) % 8 == 0)
		tamano_paginas_array = (config_swap->cantidad_paginas) / 8;
	else
		tamano_paginas_array = (config_swap->cantidad_paginas) / 8 + 1;

	char paginas_array[(int) tamano_paginas_array]; //Divido por 8 porque cada char tiene 8 bits (1 byte), creo que funciona asi la cosa //TODO: verificar los ultimos cambios
	paginas_bitmap = bitarray_create(paginas_array, sizeof paginas_array); //Creo el bitmap

	inicializar_bitmap(paginas_bitmap); //Inicializo el bitmap en 0 (false)
	log_trace(loggerManager,"Se creo e inicializo la estructura bitmap con %d cantidad de bits", bitarray_get_max_bit(paginas_bitmap));


	lista_programas = list_create(); //Aca voy a meter los t_program_info a medida que el umc me pida crear un programa

	/**************************************************************************************/


	/************************************  Server SWAP  ************************************/

	t_configuracion_servidor *servidor_swap_config = malloc(sizeof(t_configuracion_servidor));
	servidor_swap_config->puerto = config_swap->puerto_escucha;
	servidor_swap_config->funcion = atender_UMC; //No deberia hacer falta el & pero ojo
	//servidor_swap_config->parametros_funcion = config_swap;
	crear_servidor(servidor_swap_config);
	log_trace(loggerManager,"Se establecio el SWAP como servidor");

	/**************************************************************************************/


	getchar(); //pausa


	/******************************  Liberacion de recursos  *******************************/
	fclose(archivo_swap);
	log_destroy(loggerManager);
	free(config_swap->nombre_swap);
	free(config_swap);
	free(servidor_swap_config);
	bitarray_destroy(paginas_bitmap);
	list_destroy(lista_programas);

	/***************************************************************************************/

	return EXIT_SUCCESS;
}


/*********************	Función que atiende cada mensaje del UMC	*********************/

void atender_UMC(t_paquete *paquete, int socket_conexion) {

	int id_mensaje_recibido = paquete->header->id_mensaje;
	int id_proceso_emisor = paquete->header->id_proceso_emisor;
	int retorno_de_funcion;

	switch (id_mensaje_recibido) {

		case MENSAJE_HANDSHAKE:
			//TODO: Tomar valor de pagina
			log_trace(loggerManager,"[Comunicacion UMC][Mensaje recibido - cod 0] handshake");

			if(id_proceso_emisor == PROCESO_UMC)
				enviar_header_al_UMC(socket_conexion, RESPUESTA_HANDSHAKE);
			else
				handshake_error(socket_conexion);

			break;


		case MENSAJE_LEER_PAGINA_PARA_ESCRIBIR:
		case MENSAJE_LEER_PAGINA:
			//TODO: Revisar/evaluar si el umc estaba pidiendo desde la pagina 1 porque piensa que es la primera (debería de ser 0? revisar). no debería, porque a priori las lecturas les dieron resultados correctos a los chicos, si no explotaria feo
			log_trace(loggerManager,"[Comunicacion UMC][Mensaje recibido - cod 1-6] leer_pagina");

			t_pagina *pagina = malloc (sizeof (t_pagina));
			deserializar_pagina(paquete->payload, pagina);

			t_pagina_completa *pagina_completa_lectura= malloc (sizeof (t_pagina_completa)); //Se va a usar para responder ok
			pagina_completa_lectura->id_programa = pagina->id_programa;
			pagina_completa_lectura->pagina = pagina->pagina;
			pagina_completa_lectura->offset = pagina->offset;
			pagina_completa_lectura->tamanio = pagina->tamanio;
			pagina_completa_lectura->socket_pedido = pagina->socket_pedido;
			pagina_completa_lectura->valor = malloc(pagina->tamanio);


			retorno_de_funcion = leer_bytes_swap(pagina, pagina_completa_lectura->valor);


			t_buffer *buffer_pagina = serializar_pagina(pagina);
			t_buffer *buffer_pagina_completa_lectura = serializar_pagina_completa(pagina_completa_lectura);

			//Diferentes retornos de mensajes dependiendo de la situación. PD: Si quedó horrible pero el tiempo está en nuestra contra :P
			if (retorno_de_funcion != -1 && id_mensaje_recibido == MENSAJE_LEER_PAGINA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_LEER_PAGINA, buffer_pagina_completa_lectura);
			else if (retorno_de_funcion != -1 && id_mensaje_recibido == MENSAJE_LEER_PAGINA_PARA_ESCRIBIR)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR, buffer_pagina_completa_lectura);

			else if (retorno_de_funcion == -1 && id_mensaje_recibido == MENSAJE_LEER_PAGINA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_LEER_PAGINA, buffer_pagina);
			else if (retorno_de_funcion == -1 && id_mensaje_recibido == MENSAJE_LEER_PAGINA_PARA_ESCRIBIR)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_LEER_PAGINA_PARA_ESCRIBIR, buffer_pagina);

			free (pagina);
			free (pagina_completa_lectura->valor);
			free (pagina_completa_lectura);
			free (buffer_pagina->contenido_buffer);
			free (buffer_pagina);
			free (buffer_pagina_completa_lectura->contenido_buffer);
			free (buffer_pagina_completa_lectura);

			break;


		case MENSAJE_ESCRIBIR_PAGINA_NUEVA:
		case MENSAJE_ESCRIBIR_PAGINA:

			log_trace(loggerManager,"[Comunicacion UMC][Mensaje recibido - cod 2-7] escribir_pagina");
			t_pagina_completa *pagina_completa_escritura = malloc (sizeof (t_pagina_completa));
			deserializar_pagina_completa(paquete->payload , pagina_completa_escritura);

			retorno_de_funcion = escribir_bytes_swap(pagina_completa_escritura);

			t_buffer *buffer_pagina_completa_escritura = serializar_pagina_completa(pagina_completa_escritura);

			//Diferentes retornos de mensajes dependiendo de la situación. PD: Si quedó horrible pero el tiempo está en nuestra contra :P
			if (retorno_de_funcion != -1 && id_mensaje_recibido == MENSAJE_ESCRIBIR_PAGINA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_ESCRIBIR_PAGINA, buffer_pagina_completa_escritura);
			else if (retorno_de_funcion != -1 && id_mensaje_recibido == MENSAJE_ESCRIBIR_PAGINA_NUEVA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_ESCRIBIR_PAGINA_NUEVA, buffer_pagina_completa_escritura);

			else if (retorno_de_funcion == -1 && id_mensaje_recibido == MENSAJE_ESCRIBIR_PAGINA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_ESCRIBIR_PAGINA, buffer_pagina_completa_escritura);
			else if (retorno_de_funcion == -1 && id_mensaje_recibido == MENSAJE_ESCRIBIR_PAGINA_NUEVA)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_ESCRIBIR_PAGINA_NUEVA, buffer_pagina_completa_escritura);


			free (pagina_completa_escritura);
			free (buffer_pagina_completa_escritura->contenido_buffer);
			free (buffer_pagina_completa_escritura);

			break;



		case MENSAJE_INICIAR_PROGRAMA:

			log_trace(loggerManager,"[Comunicacion UMC][Mensaje recibido - cod 3] iniciar_programa");
			t_programa_nuevo *programa_nuevo = malloc (sizeof (t_programa_nuevo));
			deserializar_programa_nuevo(paquete->payload , programa_nuevo);

			retorno_de_funcion = inicializar_programa (programa_nuevo);

			t_buffer *buffer_con_programa_nuevo = serializar_programa_nuevo(programa_nuevo);

			if (retorno_de_funcion != -1)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_INICIALIZAR_PROGRAMA, buffer_con_programa_nuevo);
			else
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_INICIALIZAR_PROGRAMA, buffer_con_programa_nuevo);

			free (programa_nuevo);
			free (buffer_con_programa_nuevo->contenido_buffer);
			free (buffer_con_programa_nuevo);

			break;



		case MENSAJE_FINALIZAR_PROGRAMA:

			log_trace(loggerManager,"[Comunicacion UMC][Mensaje recibido - cod 4] finalizar_programa");

			t_programa *programa = malloc (sizeof (t_programa));
			deserializar_programa(paquete->payload, programa);

			retorno_de_funcion = finalizar_programa (programa);

			t_buffer *buffer_con_programa = serializar_programa(programa);

			if (retorno_de_funcion != -1)
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, RESPUESTA_FINALIZAR_PROGRAMA, buffer_con_programa);
			else
				enviar_mensaje_con_buffer_al_UMC(socket_conexion, ERROR_FINALIZAR_PROGRAMA, buffer_con_programa);

			free (programa);
			free(buffer_con_programa->contenido_buffer);
			free(buffer_con_programa);

			break;


		default:
			log_error(loggerManager,"[Comunicacion UMC] El código de mensaje: %i, no es un mensaje aceptado por el SWAP", id_mensaje_recibido);
			break;
	}
}

/****************************************************************************************/




/********************* Funcionalidad necesaria para atender al UMC ***********************/

/*Crea la estructura de control asociada al programa y reserva el espacio necesario en el swap, devuelve 0 si salio bien ,-2 en caso de que el programa ya exista, -1 en caso de que no haya espacio libre total disponible (no entra aunque compacte)*/
int inicializar_programa(t_programa_nuevo *inicio_programa_info){

	t_program_info *program_info = buscar_programa(inicio_programa_info->id_programa, lista_programas);

	if(program_info != NULL){ //Ya existe el programa
		log_error(loggerManager,"[Inicializacion del programa %i] El programa ya existe", inicio_programa_info->id_programa);
		return -2;
	}

	//Revisar si hay espacio total disponible libre, en caso de que no rechazar pedido
	if(!hay_espacio_total_disponible(inicio_programa_info->paginas_requeridas, paginas_bitmap)){
		log_error(loggerManager,"[Inicializacion del programa %i] No queda espacio libre en el swap como para meter el programita (aunque compacte)", inicio_programa_info->id_programa);
		return -1;
	}

	//Se busca un bloque de memoria continua donde quepa el programa
	int numero_pagina_inicial = encontrar_ubicacion_libre(inicio_programa_info->paginas_requeridas, paginas_bitmap);

	if(numero_pagina_inicial != -1){//Encontró el espacio continuo suficiente
		log_trace(loggerManager,"[Inicializacion del programa %i] Se obtuvo directamente un bloque de memoria continuo de tamano %i (paginas) en la pagina numero: %i", inicio_programa_info->id_programa, inicio_programa_info->paginas_requeridas, numero_pagina_inicial);
	}else{
		log_trace(loggerManager,"[Inicializacion del programa %i] No se encontro un bloque de memoria continuo de tamano %i (paginas), pero hay espacio suficiente, se procede a compactar", inicio_programa_info->id_programa, inicio_programa_info->paginas_requeridas);
		compactar_swap(); //TODO: Dónde joroca quedó la validación de si es suficiente el espacio libre total? No está? Revisar
		numero_pagina_inicial = encontrar_ubicacion_libre(inicio_programa_info->paginas_requeridas, paginas_bitmap);
	}
	//Se reserva el espacio encontrado (seteando 1's en el bitmap)
	reservar_lugar_para_el_programa(numero_pagina_inicial, inicio_programa_info->paginas_requeridas, paginas_bitmap);

	//Se crea la estructura de control asociada al programa
	program_info = malloc(sizeof (t_program_info));
	program_info->id_programa = inicio_programa_info->id_programa;
	program_info->cantidad_paginas = inicio_programa_info->paginas_requeridas;
	program_info->pagina_inicial_swap = numero_pagina_inicial;
	list_add(lista_programas, program_info);

	return 0;

}


/*Se encarga de eliminar la estructura de control y de liberar el espacio en el bitmap, retorna 0 en caso de exito, -1 error*/
int finalizar_programa(t_programa *fin_programa_info){

	// TODO: Analizar si esta bien que no este "borrando" (llenando con 0's)
	// el espacio previamente reservado en el archivo, no deberia hacer falta, pero tenerlo en cuenta

	t_program_info *program_info = buscar_programa(fin_programa_info->id_programa, lista_programas);

	if(program_info == NULL){
		log_error(loggerManager,"[Finalizacion del programa %i] El programa solicitado no existe", fin_programa_info->id_programa);
		return -1;
	}

	liberar_lugar_del_programa(program_info->pagina_inicial_swap, program_info->cantidad_paginas, paginas_bitmap);

	eliminar_programa(fin_programa_info->id_programa, lista_programas);

	log_trace(loggerManager,"[Finalizacion del programa %i] El programa solicitado se ha eliminado correctamente", fin_programa_info->id_programa);
	return 0;
}


/*Busca y retorna el contenido solicitado al swap*/
int leer_bytes_swap(t_pagina *leer_pagina_info, void *buffer){

	simular_espera(config_swap->retardo_acceso);

	t_program_info *programa_info = buscar_programa(leer_pagina_info->id_programa, lista_programas);

	int posicion_lectura = (programa_info->pagina_inicial_swap + leer_pagina_info->pagina) * config_swap->tamano_pagina + leer_pagina_info->offset;
	fseek(archivo_swap, posicion_lectura, SEEK_SET);

	int cantidad_leida = fread(buffer,leer_pagina_info->tamanio, 1, archivo_swap);

	if (cantidad_leida == 1){//leyo un bloque del tamano
		log_trace(loggerManager,"[Lectura de bytes] Se leyeron correctamente %i bloque de %i bytes", cantidad_leida, leer_pagina_info->tamanio);
		return 0;
	}
	else {
		log_error(loggerManager,"[Lectura de bytes] Ocurrio un problema, se leyeron %i bloques de %i bytes", cantidad_leida, leer_pagina_info->tamanio);
		return -1;
	}

}


/*Escribe en el swap el contenido de buffer, retorna 0 si el estado es ok, -1 en caso de error*/
int escribir_bytes_swap(t_pagina_completa *escribir_pagina_info){

	simular_espera(config_swap->retardo_acceso);

	t_program_info *programa_info = buscar_programa(escribir_pagina_info->id_programa, lista_programas);

	int posicion_escritura = (programa_info->pagina_inicial_swap + escribir_pagina_info->pagina) * config_swap->tamano_pagina + escribir_pagina_info->offset;
	fseek(archivo_swap, posicion_escritura, SEEK_SET);

	int cantidad_escrita = fwrite(escribir_pagina_info->valor,1, escribir_pagina_info->tamanio ,archivo_swap); //Ojo con ese 1 hardcodeado

	if (cantidad_escrita == escribir_pagina_info->tamanio){
		log_trace(loggerManager,"[Escritura de bytes] Se escribieron correctamente %i bytes en la posición %i (byte número)", cantidad_escrita, posicion_escritura);
		return 0;
	}
	else {
		log_error(loggerManager,"[Escritura de bytes] Ocurrio un problema, se escribieron %i de %i bytes", cantidad_escrita, escribir_pagina_info->tamanio);
		return -1;
	}
}

//Compacta el SWAP
void compactar_swap(){

	log_trace(loggerManager,"[Compactación] Iniciando...");
	simular_espera(config_swap->retardo_compactacion);

	int index;
	int cantidad_de_bits = bitarray_get_max_bit(paginas_bitmap);
	int contador_ceros = 0;
	t_program_info * un_programa;

	for(index = 0 ; index < cantidad_de_bits ; index ++){

		if(bitarray_test_bit(paginas_bitmap, index) && contador_ceros > 0){//1's y hubo ceros antes
			un_programa = buscar_programa_por_pagina_inicial(index, lista_programas); //TODO: Verificar que al modificarlo de este lado se modifique en la lista perse, si es un puntero debería
			reescribir_programa(un_programa, contador_ceros, archivo_swap, paginas_bitmap, config_swap, loggerManager);//Se reescribe el programa cantidad_ceros posiciones a la izquierda
			contador_ceros = 0; //Se reinicia el contador
			index = un_programa->pagina_inicial_swap + un_programa->cantidad_paginas; // Se reposiciona el index al final del programa que se movio, para seguir el algoritmo desde ahi

		}else if(!bitarray_test_bit(paginas_bitmap, index)){ //0's
			contador_ceros ++;

		}

	}

	log_trace(loggerManager,"[Compactación] Finalizado");

}

/***************************************************************************************/




/********************* Funciones de mensajeria con el UMC ***********************/

void enviar_mensaje_con_buffer_al_UMC(int socket_umc, int id_mensaje, t_buffer *t_buffers) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_SWAP;
	header->id_proceso_receptor = PROCESO_UMC;

	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = t_buffers->longitud_buffer;

	int cantidad_bytes_enviados = enviar_buffer (socket_umc, header, t_buffers);

	if (cantidad_bytes_enviados < sizeof(t_header))
		log_error(loggerManager,"[Comunicacion UMC] [%i] Ocurrió un problema al enviar el mensaje", id_mensaje);
	else
		log_trace(loggerManager,"[Comunicacion UMC] [%i] Se realizo el envio del mensaje correctamente", id_mensaje);

	free(header);
}

void enviar_header_al_UMC(int socket_umc, int id_mensaje) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_SWAP;
	header->id_proceso_receptor = PROCESO_UMC;

	header->id_mensaje = id_mensaje;
	header->longitud_mensaje = 0;

	int cantidad_bytes_enviados = enviar_header(socket_umc, header);

	if (cantidad_bytes_enviados < sizeof(t_header))
		log_error(loggerManager,"[Comunicacion UMC] [%i] Ocurrió un problema al enviar el mensaje", id_mensaje);
	else
		log_trace(loggerManager,"[Comunicacion UMC] [%i] Se realizo el envio del mensaje correctamente", id_mensaje);

	free(header);
}

void handshake_error(int socket_remitente) {
		t_header *header = malloc(sizeof(t_header));
		header->id_proceso_emisor = PROCESO_SWAP;
		header->id_proceso_receptor = -1;

		header->id_mensaje = ERROR_HANDSHAKE;
		header->longitud_mensaje = 0;

		int cantidad_bytes_enviados = enviar_header(socket_remitente, header);

		if (cantidad_bytes_enviados < sizeof(t_header))
			log_error(loggerManager,"[Comunicacion X][Respuesta Handshake erroneo] No se pudo enviar completamente");
		else
			log_trace(loggerManager,"[Comunicacion X][Respuesta Handshake erroneo] Se realizo el envio del mensaje correctamente");

		free(header);
	}

/*********************************************************************************************/
