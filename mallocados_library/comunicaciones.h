/*
 * sockets.h
 *
 *  Created on: 21/4/2016
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

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

//recibir mensaje
//enviar mensaje
//conectar a un server
//crear hilo para empezar a escuchar clientes
//crear hilo para atender clientes y seguir escuchando

void sigchld_handler(int s);

void crear_servidor(t_configuracion_servidor *config_servidor);

void escuchar_clientes(void *configuracion_escucha);

void recibir_mensaje(void *parametros_receive);

int enviar_mensaje(int socket, char *mensaje);

int conectar_servidor(char *ip, int puerto);

#endif /* SOCKETS_H_ */