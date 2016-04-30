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
#include <stdint.h>
#include <commons/config.h>
#include "nucleo.h"
#include <comunicaciones.h>

int main(void) {
	puts("Hola soy el nucleo"); /* prints proceso */

	t_config_nucleo *configuracion = malloc(sizeof(t_config_nucleo));
	cargarConfiguracionNucleo("config.nucleo.ini", configuracion);

	int socket_umc = conectar_servidor("0.0.0.0", 3603);
	enviar_mensaje(socket_umc, "Hola soy el nucleo");

	t_configuracion_servidor *configuracion_servidor = malloc(
			sizeof(t_configuracion_servidor));

	configuracion_servidor->puerto = configuracion->puerto_prog;

	crear_servidor(configuracion_servidor);

	getchar();

	return EXIT_SUCCESS;

}

void cargarConfiguracionNucleo(char *archivoConfig,
		t_config_nucleo *configuracionNucleo) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivoConfig);
	if (config_has_property(configuracion, "DEF_PUERTO_PROG")) {
		configuracionNucleo->puerto_prog = config_get_int_value(configuracion,
				"DEF_PUERTO_PROG");
	} else {
		configuracionNucleo->puerto_prog = DEF_PUERTO_PROG;
	}
	if (config_has_property(configuracion, "DEF_PUERTO_CPU")) {
		configuracionNucleo->puerto_cpu = config_get_int_value(configuracion,
				"DEF_PUERTO_CPU");
	} else {
		configuracionNucleo->puerto_cpu = DEF_PUERTO_CPU;
	}
	if (config_has_property(configuracion, "DEF_QUANTUM")) {
		configuracionNucleo->quantum = config_get_int_value(configuracion,
				"DEF_QUANTUM");
	} else {
		configuracionNucleo->quantum = DEF_QUANTUM;
	}
	if (config_has_property(configuracion, "DEF_QUANTUM_SLEEP")) {
		configuracionNucleo->quantum_sleep = config_get_int_value(configuracion,
				"DEF_QUANTUM_SLEEP");
	} else {
		configuracionNucleo->quantum_sleep = DEF_QUANTUM_SLEEP;
	}

	if (config_has_property(configuracion, "DEF_IO_ID")) {
		configuracionNucleo->io_id = config_get_array_value(configuracion,
				"DEF_IO_ID");
	} else {
		configuracionNucleo->io_id = DEF_IO_ID;
	}
	if (config_has_property(configuracion, "DEF_IO_SLEEP")) {
		configuracionNucleo->io_sleep = config_get_array_value(configuracion,
				"DEF_IO_SLEEP");
	} else {
		configuracionNucleo->io_sleep = DEF_IO_SLEEP;
	}

	if (config_has_property(configuracion, "DEF_SEM_IDS")) {
		configuracionNucleo->sem_id = config_get_array_value(configuracion,
				"DEF_SEM_IDS");
	} else {
		configuracionNucleo->io_id = DEF_SEM_IDS;
	}

	if (config_has_property(configuracion, "DEF_SEM_INIT")) {
		configuracionNucleo->sem_init = config_get_array_value(configuracion,
				"DEF_SEM_INIT");
	} else {
		configuracionNucleo->io_id = DEF_SEM_INIT;
	}

	if (config_has_property(configuracion, "DEF_SHARED_VARS")) {
		configuracionNucleo->shared_vars = config_get_array_value(configuracion,
				"DEF_SHARED_VARS");
	} else {
		configuracionNucleo->io_id = DEF_SHARED_VARS;
	}

	free(configuracion);
}

