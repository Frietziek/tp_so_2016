/*
 ============================================================================
 Name        : nucleo.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <comunicaciones.h>
#include "nucleo.h"
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <commons/string.h>

#define TRUE 1
#define MAXBUFFER 1024 // Tamanio de buffer

int main(void) {
	int sockfd;

	puts("Hola soy el nucleo"); /* prints proceso */

	t_config_nucleo *configuracion = malloc(sizeof(t_config_nucleo));
	cargarConfiguracionNucleo(
			"/home/utnso/workspace/tp-2016-1c-Los-mallocados/nucleo/src/config.nucleo.ini",
			configuracion);

	t_configuracion_servidor *configuracion_servidor = malloc(
			sizeof(t_configuracion_servidor));

	sockfd = crear_servidor(configuracion_servidor);

	//Creo estructuras para el hilo

	t_th_configuracion_escucha *configuracion_escucha = malloc(
			sizeof(t_th_configuracion_escucha));

	configuracion_escucha->socket_escucha = sockfd;
	//Creo el hilo

	pthread_t hilo_cliente;
	pthread_create(&hilo_cliente, NULL, (void*) escuchar_clientes,
			configuracion_escucha);


	return EXIT_SUCCESS;
}

void escuchar_clientes(void *configuracion) {
	t_th_configuracion_escucha *configuracion_escucha = configuracion;
	int socket_nueva_conexion; //las nuevas conexiones vienen aca
	pthread_t hilo_recibir;
	struct sockaddr_in their_addr; // info del cliente
	socklen_t sin_size;

	//ciclo hasta que lleguen conexiones nuevas
	while (TRUE) {
		sin_size = sizeof(struct sockaddr_in);
		if ((socket_nueva_conexion = accept(
				configuracion_escucha->socket_escucha,
				(struct sockaddr *) &their_addr, &sin_size)) == -1) {
			perror("accept");
			break;
		}
		printf("Recibi conexion de:  %s\n", inet_ntoa(their_addr.sin_addr));

		//Creamos estructura para mandarsela a recv
		t_th_parametros_receive *param_receive = malloc(
				sizeof(t_th_parametros_receive));
		param_receive->socket_cliente = socket_nueva_conexion;

		//creamos el hilo para recibir
		pthread_create(&hilo_recibir, NULL, (void*) recibir_mensaje,
				param_receive);

		free(param_receive);
	}
	printf("No escucho mas.\n");
	free(configuracion_escucha);
}

void recibir_mensaje(void *parametros) {
	t_th_parametros_receive *parametros_receive = parametros;
	int Tipo_Conexion;
	char buffer[MAXBUFFER];

	while (TRUE) {
		buffer[0] = '\0';

		///Hang shake

		// Recibo mensaje del cliente
		if (recibir_mensaje(parametros_receive->socket_cliente, buffer)){
			break;
		}

		Tipo_Conexion = buffer[0];

		//Determina que tipo de proceso se conecto

		switch (Tipo_Conexion){
			case CONSOLA:
				// Envia Confirmación Hand shake
				buffer [0] = OK_CONSOLA;
				buffer [1] = '\0';
				if (enviar_mensaje(parametros_receive->socket_cliente, buffer)){
					printf ("Conexion Consola Realizada");
				}

				////////////Donde Recive el script/////////////////////

				break;
			case CPU:
				// Envia Confirmación Hand shake
				buffer [0] = OK_CPU;
				buffer [1] = '\0';
				if (enviar_mensaje(parametros_receive->socket_cliente, buffer)){
					printf ("Conexion CPU Realizada");
				}
				break;
			case UMC:
				// Envia Confirmación Hand shake
				buffer [0] = OK_UMC;
				buffer [1] = '\0';
				if (enviar_mensaje(parametros_receive->socket_cliente, buffer)){
					printf ("Conexion UMC Realizada");
				}
				break;
		}
		break;
	}
	// Cierro el socket
	printf("Se cerro la conexion.\n");
	close(parametros_receive->socket_cliente);
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

