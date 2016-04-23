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
	int puerto;
	int socket_escucha;
} t_configuracion_escucha;

typedef struct {
	int socket_cliente;
	char *ip_cliente;
	int puerto;
} t_parametros_receive;

//recibir mensaje
//enviar mensaje
//conectar a un server
//crear hilo para empezar a escuchar clientes
//crear hilo para atender clientes y seguir escuchando

void sigchld_handler(int s);

void prepara_escucha(t_configuracion_servidor *config_servidor);

void escuchar_clientes(t_configuracion_escucha *configuracion_escucha);

void recibir_mensaje(t_parametros_receive *parametros_receive);

int enviar_mensaje(int socket, char *mensaje);

#endif /* SOCKETS_H_ */
