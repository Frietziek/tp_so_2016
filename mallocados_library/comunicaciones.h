/*
 * sockets.h
 *
 *  Created on: 21/4/2016
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <commons/collections/dictionary.h>
#include "serializacion.h"

typedef struct {
	int puerto;
	t_dictionary *funcion;
	void *parametros_funcion;
} t_configuracion_servidor;

typedef struct {
	pthread_t thread_id;
	int socket_escucha;
	void *funcion;
	void *parametros_funcion;
} t_th_configuracion_escucha;

typedef struct {
	pthread_t thread_id;
	int socket_cliente;
	void *funcion;
	void *parametros_funcion;
	void *buffer;
} t_th_parametros_receive;

void sigchld_handler(int s);

void crear_servidor(t_configuracion_servidor *config_servidor);

void escuchar_clientes(void *configuracion_escucha);

void recibir_mensaje(void *parametros_receive);

int enviar_buffer(int socket, t_buffer *buffer);

int enviar_mensaje(int socket, char *mensaje);

int conectar_servidor(char *ip, int puerto);

#endif /* SOCKETS_H_ */
