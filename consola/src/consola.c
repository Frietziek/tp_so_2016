/*
 ============================================================================
 Name        : consola.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "consola.h"
//#include <parser/metadata_program.h>
#include <comunicaciones.h>



int main(void) {



	t_config_consola *configuracion = malloc(sizeof(t_config_consola)); // Estructura de configuracion de la UMC
	cargaConfiguracionConsola("config.consola.ini", configuracion);

	printf("Proceso Consola creado.\n");



	int socket_nucleo = conectar_servidor("0.0.0.0", 5000);
	printf("Consola conectada con el Núcleo.\n");
	enviar_mensaje(socket_nucleo, "Hola soy la consola");


	getchar();


	close(socket_nucleo);
	free(configuracion);
	return EXIT_SUCCESS;
}

void cargaConfiguracionConsola(char *archivo, t_config_consola *configuracionConsola) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracionConsola->puerto = config_get_int_value(configuracion, "PUERTO_NUCLEO");
	} else {
		configuracionConsola->puerto = DEF_PUERTO_Nucleo;
	}
	if (config_has_property(configuracion, "IP_NUCLEO")) {
		configuracionConsola->ip = config_get_string_value(configuracion, "IP_NUCLEO");
	} else {
		configuracionConsola->ip = DEF_IP_Nucleo;
	}
	free(configuracion);
}
