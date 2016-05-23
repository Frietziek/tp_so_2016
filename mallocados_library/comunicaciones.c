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

#define TRUE 1
#define BACKLOG 10     // Cuántas conexiones pendientes se mantienen en cola
#define MAXBUFFER 1024 // Tamanio de buffer

void sigchld_handler(int s) {
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

	//Creo estructuras para el hilo

	t_th_configuracion_escucha *configuracion_escucha = malloc(
			sizeof(t_th_configuracion_escucha));

	configuracion_escucha->socket_escucha = sockfd;
	configuracion_escucha->funcion = config_servidor->funcion;
	configuracion_escucha->parametros_funcion =
			config_servidor->parametros_funcion;

	//Creo el hilo

	pthread_t hilo_cliente;
	pthread_create(&hilo_cliente, NULL, (void*) escuchar_clientes,
			configuracion_escucha);

}

void escuchar_clientes(void *configuracion) {
	t_th_configuracion_escucha *configuracion_escucha = configuracion;
	int socket_nueva_conexion; //las nuevas conexiones vienen aca
	pthread_t hilo_funcion;
	struct sockaddr_in their_addr; // info del cliente
	socklen_t sin_size;

	//Creamos estructura para mandarsela a recv
	t_th_parametros_receive *param_receive = malloc(
			sizeof(t_th_parametros_receive));

	param_receive->funcion = configuracion_escucha->funcion;
	param_receive->parametros_funcion =
			configuracion_escucha->parametros_funcion;

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

		param_receive->socket_cliente = socket_nueva_conexion;

		//creamos el hilo para recibir
		pthread_create(&hilo_funcion, NULL, (void*) recibir_mensaje,
				param_receive);

	}
	printf("No escucho mas.\n");
	free(configuracion_escucha);
	free(param_receive);
}

void recibir_mensaje(void *parametros) {
	t_th_parametros_receive *parametros_receive = parametros;
	int bytes_recibidos;
	char buffer[MAXBUFFER];

	while (TRUE) {
		buffer[0] = '\0';

		// Recibo mensaje del cliente
		if ((bytes_recibidos = recv(parametros_receive->socket_cliente, buffer,
		MAXBUFFER - 1, 0)) == -1) {
			perror("recv");
			break;
		}

		// Verifico que el cliente no haya cerrado la conexion
		if (bytes_recibidos == 0) {
			perror("el socket cliente cerro la conexion\n");
			break;
		} else {

			if (parametros_receive->funcion == NULL) {
				buffer[bytes_recibidos] = '\0';
				printf("Mensaje recibido: %s\n", buffer);
			} else {
				if (parametros_receive->parametros_funcion == NULL) {
					//funcion sin parametros, solo el buffer como parametro
					void (*fn)() = parametros_receive->funcion;
					fn(buffer);

				} else {
					//funcion con parametros, ademas del buffer como parametro
					void (*fn)() = parametros_receive->funcion;
					void (*parametro) = parametros_receive->parametros_funcion;

					fn(parametro, buffer);

				}
			}

		}

	}

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

int enviar_header(int socket, t_header *header) {
	int bytes_enviados_totales = 0;
	int bytes_enviados = 0;

	t_buffer *paquete_serializado = serializar_header(header);

//ciclo hasta que se enviee toodo lo que quiero enviar
	if (socket >= 0) {
		while (bytes_enviados_totales < paquete_serializado->longitud_buffer) {
			if ((bytes_enviados = send(socket,
					paquete_serializado->contenido_buffer,
					paquete_serializado->longitud_buffer, 0)) == -1) {
				perror("send");
				close(socket);
				return -1;
			}
			bytes_enviados_totales += bytes_enviados;
		}
	}
	free(paquete_serializado);
	return bytes_enviados_totales;
}

int enviar_buffer(int socket, t_header *header, t_buffer *buffer) {
	int bytes_enviados_totales = 0;
	int bytes_enviados = 0;

	int cantidad_a_enviar = sizeof(t_header) + buffer->longitud_buffer;

	void *paquete_serializado = serializar_con_header(header, buffer);

	int id_p_e;
	int pos_buffer = 0;

	escribir_atributo_desde_int_de_buffer(paquete_serializado, &(id_p_e),
			&pos_buffer);

	printf("\n enviar_buffer\n");
	printf(
			"el id_p_e que estoy serializando en buffer tiene un valor de: %d\n\n",
			id_p_e);

//ciclo hasta que se enviee toodo lo que quiero enviar
	if (socket >= 0) {
		while (bytes_enviados_totales < cantidad_a_enviar) {
			if ((bytes_enviados = send(socket, paquete_serializado,
					cantidad_a_enviar, 0)) == -1) {
				perror("send");
				close(socket);
				return -1;
			}
			bytes_enviados_totales += bytes_enviados;
		}
	}
	free(paquete_serializado);
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
	addr_server.sin_family = AF_INET; // Ordenación de bytes de la máquina
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
	pthread_create(&hilo_recibir, NULL, (void*) recibir_mensaje, param_receive);

	return socket_cliente;
}
