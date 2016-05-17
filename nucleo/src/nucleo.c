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
#include <comunicaciones.h>
#include "nucleo.h"
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <serializacion.h>

int main(void) {

	puts("Hola soy el nucleo"); /* prints proceso */

	t_config_nucleo *configuracion = malloc(sizeof(t_config_nucleo));
	cargarConfiguracionNucleo("src/config.nucleo.ini", configuracion);

	int socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc);
	enviar_mensaje(socket_umc, "Hola soy el nucleo");

	t_configuracion_servidor *configuracion_servidor = malloc(
			sizeof(t_configuracion_servidor));

	configuracion_servidor->puerto = configuracion->puerto_prog;

	//ESTO ES LO QUE HAY QUE HACER, EN configuracion_servidor->funcion  PONER &nombre_de_funcion
	//Y EN configuracion_servidor->parametros_funcion LO QUE DEBERIA RECIBIR LA FUNCION

	configuracion_servidor->funcion = &funcion_saludar;
	configuracion_servidor->parametros_funcion = configuracion;

	crear_servidor(configuracion_servidor);

	getchar();

	free(configuracion);
	free(configuracion_servidor);

	return EXIT_SUCCESS;
}

void funcion_saludar(t_config_nucleo*config, void *buffer) {

	printf(
			"hola mundo, soy el Reno Jose, el puerto del cpu es %d, el primer semaforo es: %s\n",
			config->puerto_cpu, config->sem_id[0]);

	//printf("hola mundo, soy el Reno Jose, el buffer contiene: %s \n", buffer);

	t_persona *persona2 = malloc(sizeof(t_persona));

	deserializar_persona(buffer, persona2);

	printf("el apellido de la persona es: %s\n", persona2->apellido);

	free(persona2);

}

void cargarConfiguracionNucleo(char *archivoConfig,
		t_config_nucleo *configuracion_nucleo) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivoConfig);
	if (config_has_property(configuracion, "DEF_PUERTO_PROG")) {
		configuracion_nucleo->puerto_prog = config_get_int_value(configuracion,
				"DEF_PUERTO_PROG");
	} else {
		perror("error al cargar DEF_PUERTO_PROG");
	}
	if (config_has_property(configuracion, "DEF_PUERTO_CPU")) {
		configuracion_nucleo->puerto_cpu = config_get_int_value(configuracion,
				"DEF_PUERTO_CPU");
	} else {
		perror("error al cargar DEF_PUERTO_CPU");
	}
	if (config_has_property(configuracion, "DEF_QUANTUM")) {
		configuracion_nucleo->quantum = config_get_int_value(configuracion,
				"DEF_QUANTUM");
	} else {
		perror("error al cargar DEF_QUANTUM");
	}
	if (config_has_property(configuracion, "DEF_QUANTUM_SLEEP")) {
		configuracion_nucleo->quantum_sleep = config_get_int_value(
				configuracion, "DEF_QUANTUM_SLEEP");
	} else {
		perror("error al cargar DEF_QUANTUM_SLEEP");
	}

	if (config_has_property(configuracion, "DEF_IO_ID")) {
		configuracion_nucleo->io_id = config_get_array_value(configuracion,
				"DEF_IO_ID");
	} else {
		perror("error al cargar DEF_IO_ID");
	}
	if (config_has_property(configuracion, "DEF_IO_SLEEP")) {
		configuracion_nucleo->io_sleep = config_get_array_value(configuracion,
				"DEF_IO_SLEEP");
	} else {
		perror("error al cargar DEF_IO_SLEEP");
	}

	if (config_has_property(configuracion, "DEF_SEM_IDS")) {
		configuracion_nucleo->sem_id = config_get_array_value(configuracion,
				"DEF_SEM_IDS");
	} else {
		perror("error al cargar DEF_SEM_IDS");
	}

	if (config_has_property(configuracion, "DEF_SEM_INIT")) {
		configuracion_nucleo->sem_init = config_get_array_value(configuracion,
				"DEF_SEM_INIT");
	} else {
		perror("error al cargar DEF_SEM_INIT");
	}

	if (config_has_property(configuracion, "DEF_SHARED_VARS")) {
		configuracion_nucleo->shared_vars = config_get_array_value(
				configuracion, "DEF_SHARED_VARS");
	} else {
		perror("error al cargar DEF_SHARED_VARS");
	}

	if (config_has_property(configuracion, "DEF_IP_UMC")) {
		configuracion_nucleo->ip_umc = config_get_string_value(configuracion,
				"DEF_IP_UMC");
	} else {
		perror("error al cargar DEF_IP_UMC");
	}

	if (config_has_property(configuracion, "DEF_PUERTO_UMC")) {
		configuracion_nucleo->puerto_umc = config_get_int_value(configuracion,
				"DEF_PUERTO_UMC");
	} else {
		perror("error al cargar DEF_PUERTO_UMC");
	}

	free(configuracion);
}

