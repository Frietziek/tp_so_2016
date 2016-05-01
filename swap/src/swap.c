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
#include <commons/config.h>
#include <commons/log.h>
#include "tipos_swap.h"
#include "utilidades_swap.h"
#include <comunicaciones.h>

int main(void) {
	//Creo un archivo log
	t_log *loggerManager = 	log_create("swap.log","SWAP", true, LOG_LEVEL_TRACE);

	//Cargo las configuraciones del SWAP
	t_config_swap *config_swap = malloc(sizeof(t_config_swap));
	cargar_configuracion_swap("src/config.swap.ini",config_swap);
	log_trace(loggerManager, "\nSe cargaron las configuraciones con los siguientes valores: \nPUERTO_ESCUCHA=%i \nNOMBRE_SWAP=%s\nCANTIDAD_PAGINAS=%i\nTAMANO_PAGINA=%i\nRETARDO_COMPACTACION=%i\n", config_swap->puerto_escucha, config_swap->nombre_swap, config_swap->cantidad_paginas, config_swap->tamano_pagina, config_swap->retardo_compactacion);

	//TODO: Crear particion del swap

	//Se levanta un servidor que va a escuchar las peticiones del UMC
	t_configuracion_servidor *servidor_swap_config = malloc(sizeof(t_configuracion_servidor));
	servidor_swap_config->puerto = config_swap->puerto_escucha;
	crear_servidor(servidor_swap_config);
	printf("Se establecio el SWAP como servidor");

	getchar();
	//Cierro el archivo de log
	log_destroy(loggerManager);


	return EXIT_SUCCESS;
}
