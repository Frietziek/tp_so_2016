/*
 * sockets.c
 *
 *  Created on: 21/4/2016
 *      Author: utnso
 */

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
#include "comunicaciones.h"

#define MYPORT 3490    // Puerto al que conectarán los usuarios
#define TRUE 1
#define BACKLOG 10     // Cuántas conexiones pendientes se mantienen en cola
#define MAXBUFFER 1024 // Tamanio de buffer

int sigchld_handler(int s) {
	while (wait(NULL) > 0)
		;
}

void crear_servidor(t_configuracion_servidor *config_servidor) {
	int sockfd; // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sigaction sa;
	int yes = 1;

	//Creamos el socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	//Cierra el puerto por si quedo abierto
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	//Creamos la estructura para el bind
	my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(config_servidor->puerto); // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	//Asocio el puerto con el socket
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);
	}

	//Me pongo a escuchar
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	// Eliminar procesos muertos
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	return sockfd;

}


int enviar_mensaje(int socket, char *mensaje) {
	int bytes_enviados_totales = 0;
	int bytes_enviados = 0;
	int longitud_mensaje = string_length(mensaje);

	//ciclo hasta que se enviee toodo lo que quiero enviar
	if (socket >= 0) {
		while (bytes_enviados_totales < longitud_mensaje) {
			if ((bytes_enviados = send(socket, mensaje, longitud_mensaje, 0))
					== -1) {
				perror("send");
				close(socket);
				return -1;
			}
			bytes_enviados_totales += bytes_enviados;
		}
	}

	return bytes_enviados_totales;
}

int conectar_servidor(char *ip, int puerto) {

	int socket_cliente;
	struct hostent *he;
	// información del destino
	struct sockaddr_in addr_server;
	pthread_t hilo_recibir;

	if ((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		return -1;
	}

	//Abrimos socket
	if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	//Configuración de dirección de servidor
	addr_server.sin_family = AF_INET;    // Ordenación de bytes de la máquina
	addr_server.sin_port = htons(puerto); // short, Ordenación de bytes de la red
	addr_server.sin_addr = *((struct in_addr *) he->h_addr);
	memset(&(addr_server.sin_zero), 0, 8); // poner a cero el resto de la estructura

	//Conecto con servidor, si hay error finalizo
	if (connect(socket_cliente, (struct sockaddr *) &addr_server,
			sizeof(struct sockaddr)) == -1) {
		perror("connect");
		return -1;
	}

	//Creamos estructura para mandarsela a recv
	t_th_parametros_receive *param_receive = malloc(
			sizeof(t_th_parametros_receive));
	param_receive->socket_cliente = socket_cliente;

	//creamos el hilo para recibir
	pthread_create(&hilo_recibir, NULL, (void*) recibir_mensaje,
			param_receive);

	return socket_cliente;
}
