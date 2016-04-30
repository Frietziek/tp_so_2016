/*
 * sockets.h
 *
 *  Created on: 21/4/2016
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#define CONSOLA 1
#define OK_CONSOLA 2
#define CPU 1
#define OK_CPU 2
#define UMC 1
#define OK_UMC 2

typedef struct {
	int puerto;
} t_configuracion_servidor;

typedef struct {
	pthread_t thread_id;
	int socket_escucha;
} t_th_configuracion_escucha;

typedef struct {
	pthread_t thread_id;
	int socket_cliente;
} t_th_parametros_receive;

void sigchld_handler(int s);

int crear_servidor(t_configuracion_servidor *config_servidor);

int recibir_mensaje(int socket, char *buffer)

void escuchar_clientes(void *configuracion_escucha);

void recibir_mensaje(void *parametros_receive);

int enviar_mensaje(int socket, char *mensaje);

int conectar_servidor(char *ip, int puerto);

#endif /* SOCKETS_H_ */
