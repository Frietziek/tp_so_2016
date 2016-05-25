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


int main(int argc, char **argv) {


	int index;
	FILE *archivo;
	char Aux_Archivo[100];
	t_config_consola *configuracion = malloc(sizeof(t_config_consola)); // Estructura de configuracion de la consola
	cargaConfiguracionConsola("src/config.consola.ini", configuracion);

	printf("Proceso Consola creado.\n");



	int socket_nucleo = conectar_servidor(configuracion->ip, configuracion->puerto);
	printf("Consola conectada con el Núcleo.\n");
	enviar_mensaje(socket_nucleo, "Hola soy la consola");

	// Lee Archivo y envia el archivo Ansisop
	archivo = fopen(argv[1], "r"); //El argv[1] tiene la direccion del archivo Ansisop
	if (archivo == NULL)   exit(1);
	//Envia linea por linea el archivo Ansisop
	while (feof(archivo) == 0){
		fgets(Aux_Archivo,100,archivo);
		enviar_mensaje(socket_nucleo, Aux_Archivo);
	}


	getchar();

	fclose( archivo);
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




