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
#include <comunicaciones.h>
#include "tipos_swap.h"
#include "utilidades_swap.h"


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
		log_trace(loggerManager,"Hubo un problema al intentar crear el archivo swap");
	/**************************************************************************************/


	/************************************  Server SWAP  ************************************/
	t_configuracion_servidor *servidor_swap_config = malloc(sizeof(t_configuracion_servidor));
	servidor_swap_config->puerto = config_swap->puerto_escucha;
	crear_servidor(servidor_swap_config);
	log_trace(loggerManager,"Se establecio el SWAP como servidor");
	/**************************************************************************************/

	getchar(); //pausa

	/******************************  Liberacion de recursos  *******************************/
	log_destroy(loggerManager); //cierro el archivo de log
	free(config_swap);
	free(servidor_swap_config);
	/***************************************************************************************/

	return EXIT_SUCCESS;
}
