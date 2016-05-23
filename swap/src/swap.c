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

/*****************************Declaracion de funciones del SWAP********************************/
int inicializar_programa(t_inicio_programa *inicio_programa_info, t_list *lista_programas, t_bitarray *paginas_bitmap, t_log *loggerManager);
int finalizar_programa(t_fin_programa *fin_programa_info, t_bitarray *paginas_bitmap, t_list *lista_programas, t_log *loggerManager);
int leer_bytes_swap(t_leer_pagina *leer_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager, void *buffer);
int escribir_bytes_swap(t_escribir_pagina *escribir_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager);
/**********************************************************************************************/

int main(void) {

	t_log *loggerManager = 	log_create("swap.log","SWAP", true, LOG_LEVEL_TRACE); //creo un archivo log


	/********************************  Configuraciones SWAP  ********************************/

	t_config_swap *config_swap = malloc(sizeof(t_config_swap));
	cargar_configuracion_swap("src/config.swap.ini",config_swap);
	log_trace(loggerManager, "\nSe cargaron las configuraciones con los siguientes valores: \nPUERTO_ESCUCHA=%i \nNOMBRE_SWAP=%s\nCANTIDAD_PAGINAS=%i\nTAMANO_PAGINA=%i\nRETARDO_COMPACTACION=%i\n", config_swap->puerto_escucha, config_swap->nombre_swap, config_swap->cantidad_paginas, config_swap->tamano_pagina, config_swap->retardo_compactacion);

	/***************************************************************************************/


	/***********************************  Archivo SWAP  ***********************************/

	int status = crear_archivo_swap(config_swap->nombre_swap, config_swap->cantidad_paginas * config_swap->tamano_pagina); //creo e inicializo el archivo SWAP
	if(status!=-1)
		log_trace(loggerManager,"Se creo correctamente el archivo swap");
	else
		log_error(loggerManager,"Hubo un problema al intentar crear el archivo swap");

	FILE *archivo_swap = fopen(config_swap->nombre_swap, "rb+");

	/**************************************************************************************/


	/******************************  Estructuras de control  ******************************/

	t_bitarray *paginas_bitmap = malloc(sizeof paginas_bitmap); //Este es el bitmap perse
	char paginas_array[(config_swap->cantidad_paginas)/8]; //Divido por 8 porque cada char tiene 8 bits (1 byte), creo que funciona asi la cosa
	paginas_bitmap = bitarray_create(paginas_array, sizeof paginas_array); //Creo el bitmap

	inicializar_bitmap(paginas_bitmap); //Inicializo el bitmap en 0 (false)
	log_trace(loggerManager,"Se creo e inicializo la estructura bitmap con %d cantidad de bits", bitarray_get_max_bit(paginas_bitmap));


	t_list *lista_programas = list_create(); //Aca voy a meter los t_program_info a medida que el umc me pida crear un programa

	/**************************************************************************************/


	/************************************  Server SWAP  ************************************/

	t_configuracion_servidor *servidor_swap_config = malloc(sizeof(t_configuracion_servidor));
	servidor_swap_config->puerto = config_swap->puerto_escucha;
	crear_servidor(servidor_swap_config);
	log_trace(loggerManager,"Se establecio el SWAP como servidor");

	/**************************************************************************************/


	/*******  Pruebas funciones SWAP (mantener comentado, descomentar para testear) *******/

	/*t_inicio_programa *inicio_programa_info1 = malloc(sizeof (t_inicio_programa));
	inicio_programa_info1->id_programa = 20;
	inicio_programa_info1->paginas_requeridas = 4;

	t_inicio_programa *inicio_programa_info2 = malloc(sizeof (t_inicio_programa));
	inicio_programa_info2->id_programa = 21;
	inicio_programa_info2->paginas_requeridas = 10;

	t_inicio_programa *inicio_programa_info3 = malloc(sizeof (t_inicio_programa));
	inicio_programa_info3->id_programa = 25;
	inicio_programa_info3->paginas_requeridas = 20;

	t_inicio_programa *inicio_programa_info4 = malloc(sizeof (t_inicio_programa));
	inicio_programa_info4->id_programa = 20;
	inicio_programa_info4->paginas_requeridas = 10;

	t_fin_programa *fin_programa_info1 = malloc(sizeof (t_fin_programa));
	fin_programa_info1->id_programa = 20;

	t_fin_programa *fin_programa_info2 = malloc(sizeof (t_fin_programa));
	fin_programa_info2->id_programa = 21;

	t_fin_programa *fin_programa_info3 = malloc(sizeof (t_fin_programa));
	fin_programa_info3->id_programa = 30;

	inicializar_programa(inicio_programa_info1, lista_programas, paginas_bitmap, loggerManager);
	inicializar_programa(inicio_programa_info2, lista_programas, paginas_bitmap, loggerManager);
	inicializar_programa(inicio_programa_info4, lista_programas, paginas_bitmap, loggerManager);
	finalizar_programa(fin_programa_info1, paginas_bitmap, lista_programas, loggerManager);
	finalizar_programa(fin_programa_info2, paginas_bitmap, lista_programas, loggerManager);
	finalizar_programa(fin_programa_info3, paginas_bitmap, lista_programas, loggerManager);
	inicializar_programa(inicio_programa_info3, lista_programas, paginas_bitmap, loggerManager);

	free(inicio_programa_info1);
	free(inicio_programa_info2);
	free(inicio_programa_info3);
	free(fin_programa_info1);
	free(fin_programa_info2);
	free(fin_programa_info3);*/

	/*
	//lectura inicial
	t_leer_pagina *leer_pagina_info = malloc(sizeof(t_leer_pagina));
	leer_pagina_info->tamanio = 10;
	leer_pagina_info->pagina = 0;
	leer_pagina_info->offset = 1;
	void *buffer = malloc(leer_pagina_info->tamanio);
	leer_bytes_swap(leer_pagina_info, archivo_swap, config_swap, loggerManager, buffer);

	//escritura
	char str[] = "This is tutorialspoint.com";
	t_escribir_pagina *escribir_pagina_info = malloc(sizeof(t_escribir_pagina));
	escribir_pagina_info->buffer = str;
	escribir_pagina_info->tamanio = sizeof(str);
	escribir_pagina_info->pagina = 1;
	escribir_pagina_info->offset = 1;
	escribir_bytes_swap(escribir_pagina_info, archivo_swap, config_swap, loggerManager);

	//lectura 2
	leer_bytes_swap(leer_pagina_info, archivo_swap, config_swap, loggerManager, buffer);

	free(leer_pagina_info);
	free(escribir_pagina_info);
	*/

	/**************************************************************************************/


	getchar(); //pausa


	/******************************  Liberacion de recursos  *******************************/

	log_destroy(loggerManager);
	free(config_swap);
	free(servidor_swap_config);
	bitarray_destroy(paginas_bitmap);
	list_destroy(lista_programas);

	/***************************************************************************************/

	return EXIT_SUCCESS;
}


//--------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------//


/*Crea la estructura de control asociada al programa y reserva el espacio necesario en el swap, devuelve 0 si salio bien y -1 en caso de que de error al buscar bloque y -2 en caso de que el programa ya exista*/
int inicializar_programa(t_inicio_programa *inicio_programa_info, t_list *lista_programas, t_bitarray *paginas_bitmap, t_log *loggerManager){

	t_program_info *program_info = buscar_programa(inicio_programa_info->id_programa, lista_programas);

	if(program_info != NULL){ //Ya existe el programa
		log_error(loggerManager,"[Inicializacion del programa %i] El programa ya existe", inicio_programa_info->id_programa);
		return -2;
	}

	//Se busca un bloque de memoria continua donde quepa el programa
	int numero_pagina_inicial = encontrar_ubicacion_libre(inicio_programa_info->paginas_requeridas, paginas_bitmap);

	if(numero_pagina_inicial != -1){
		log_trace(loggerManager,"[Inicializacion del programa %i] Se obtuvo directamente un bloque de memoria continuo de tamano %i (paginas) en la pagina numero: %i", inicio_programa_info->id_programa, inicio_programa_info->paginas_requeridas, numero_pagina_inicial);
	}else{
		log_error(loggerManager,"[Inicializacion del programa %i] No se encontro un bloque de memoria continuo de tamano %i (paginas)", inicio_programa_info->id_programa, inicio_programa_info->paginas_requeridas);
		return -1;
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
int finalizar_programa(t_fin_programa *fin_programa_info, t_bitarray *paginas_bitmap, t_list *lista_programas, t_log *loggerManager){

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
int leer_bytes_swap(t_leer_pagina *leer_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager, void *buffer){

	//TODO: Leer en la (posicion pagina * tamano de pagina) + offset una cantidad

	int posicion_lectura = leer_pagina_info->pagina * config_swap->tamano_pagina + leer_pagina_info->offset;
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
int escribir_bytes_swap(t_escribir_pagina *escribir_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager){

	//TODO: No se esta evaluando si es correcto que se escriba en esta region de memoria, evaluar eso despues

	int posicion_escritura = escribir_pagina_info->pagina * config_swap->tamano_pagina + escribir_pagina_info->offset;
	fseek(archivo_swap, posicion_escritura, SEEK_SET);

	int cantidad_escrita = fwrite(escribir_pagina_info->buffer,1, escribir_pagina_info->tamanio ,archivo_swap); //Ojo con ese 1 hardcodeado

	if (cantidad_escrita == escribir_pagina_info->tamanio){
		log_trace(loggerManager,"[Escritura de bytes] Se escribieron correctamente %i bytes", cantidad_escrita);
		return 0;
	}
	else {
		log_error(loggerManager,"[Escritura de bytes] Ocurrio un problema, se escribieron %i de %i bytes", cantidad_escrita, escribir_pagina_info->tamanio);
		return -1;
	}
}

