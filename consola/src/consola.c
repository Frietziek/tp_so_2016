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

#define MAXBUFFER 10

void cargaConfiguracionConsola(char *archivo, t_config_consola *configuracion);

int main(void) {

	char buffer[MAXBUFFER], Verificación_Consola;
	int bytes_recibidos;
	t_config_consola *configuracion = malloc(sizeof(t_config_consola)); // Estructura de configuracion de la UMC
	cargaConfiguracionConsola("/home/utnso/workspace/tp-2016-1c-Los-mallocados/consola/src/config.consola.ini", configuracion);

	printf("Proceso Consola creado.\n");

	int socket_consola = conectar_servidor(configuracion->ip, configuracion->puerto);
	buffer [0] = CONSOLA;
	buffer [1] = "/0";
	enviar_mensaje(socket_consola, buffer);

	// Recibo mensaje del Servidor
	if ((bytes_recibidos = recv(socket_consola, buffer,
	MAXBUFFER - 1, 0)) == -1) {
		perror("recv");
		break;
	}

	// Verifico que el cliente no haya cerrado la conexion
	if (bytes_recibidos == 0) {
		break;
	} else {
		// TODO Llamar funciones correspondientes
		buffer[bytes_recibidos] = '\0';
		Verificación_Consola = buffer[0];
	}

	//Verifica el Hang Shake

	if (Verificación_Consola == OK_CONSOLA){
		printf ("Consola Conectada al Nucleo");
	}

	////////////Envía el script/////////////////////

	close(socket_consola);
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
