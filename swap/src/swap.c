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

int main(void) {
	//Creo un archivo log
	t_log *loggerManager = 	log_create("swap.log","SWAP", true, LOG_LEVEL_TRACE);

	//Cargo las configuraciones del SWAP
	t_config_swap *config_swap = malloc(sizeof(t_config_swap));
	cargar_configuracion_swap("/home/utnso/Desktop/home/workspace/tp-2016-1c-Los-mallocados/swap/src/config.swap.ini",config_swap);
	log_trace(loggerManager, "\nSe cargaron las configuraciones con los siguientes valores: \nPUERTO_ESCUCHA=%i \nNOMBRE_SWAP=%s\nCANTIDAD_PAGINAS=%i\nTAMANO_PAGINA=%i\nRETARDO_COMPACTACION=%i\n", config_swap->puerto_escucha, config_swap->nombre_swap, config_swap->cantidad_paginas, config_swap->tamano_pagina, config_swap->retardo_compactacion);


	return EXIT_SUCCESS;
}
