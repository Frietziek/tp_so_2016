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

#define TRUE  1
#define FALSE  0
#define BACKLOG  10     // Cuántas conexiones pendientes se mantienen en cola

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

	//ACA PREGUNTO SI LOS PARAMETROS SON NULL

	if (config_servidor->parametros_funcion != NULL) {
		configuracion_escucha->parametros_funcion =
				config_servidor->parametros_funcion;
	}

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

	//ciclo hasta que lleguen conexiones nuevas
	while (TRUE) {
		sin_size = sizeof(struct sockaddr_in);
		if ((socket_nueva_conexion = accept(
				configuracion_escucha->socket_escucha,
				(struct sockaddr *) &their_addr, &sin_size)) == -1) {
			perror("accept");
			break;
		}

		//Creamos estructura para mandarsela a recv
		t_th_parametros_receive *param_receive = malloc(
				sizeof(t_th_parametros_receive));
		param_receive->socket_cliente = socket_nueva_conexion;

		param_receive->funcion = configuracion_escucha->funcion;

		//ACA PREGUNTO SI LOS PARAMETROS SON NULL

		if (configuracion_escucha->parametros_funcion != NULL) {
			param_receive->parametros_funcion =
					configuracion_escucha->parametros_funcion;
		}


		//creamos el hilo para recibir
		pthread_create(&hilo_funcion, NULL, (void*) recibir_mensaje,
				param_receive);

	}
	printf("No escucho mas.\n");
	free(configuracion_escucha);

}

void recibir_mensaje(t_th_parametros_receive *parametros) {
	t_th_parametros_receive *parametros_receive = parametros;
	int bytes_recibidos_header;
	int bytes_recibidos_payload;
	char buffer_header[sizeof(t_header)];
	int estoy_conectado = TRUE;

	while (estoy_conectado) {
		buffer_header[0] = '\0';

		// Recibo header
		if ((bytes_recibidos_header = recv(parametros_receive->socket_cliente,
				buffer_header, sizeof(t_header), 0)) == -1) {
			estoy_conectado = FALSE;
			perror("recv header");
			free(parametros);
			break;
		}

		// Verifico que el cliente no haya cerrado la conexion
		if (bytes_recibidos_header == 0) {
			perror("el socket cliente cerro la conexion\n");
			estoy_conectado = FALSE;
			break;
		} else {
			t_paquete *paquete = malloc(sizeof(t_paquete));

			paquete->header = malloc(sizeof(t_header));
			deserializar_header(buffer_header, paquete->header);

			if (paquete->header->longitud_mensaje > 0) {

				paquete->payload = malloc(paquete->header->longitud_mensaje);
				// Recibo payload
				if ((bytes_recibidos_payload = recv(
						parametros_receive->socket_cliente, paquete->payload,
						paquete->header->longitud_mensaje, 0)) == -1) {
					estoy_conectado = FALSE;
					perror("recv payload");
					break;
				}
			} else {
				//printf("\n payload vacio\n");
			}

			void (*fn)() = parametros_receive->funcion;

			//ACA PREGUNTO SI LOS PARAMETROS SON NULL

			if (parametros_receive->parametros_funcion != NULL) {

				fn(paquete, parametros_receive->socket_cliente,
						parametros_receive->parametros_funcion);

			} else {
				fn(paquete, parametros_receive->socket_cliente);
			}

			if (paquete->header->longitud_mensaje > 0) {
				free(paquete->payload);
			}
			free(paquete->header);
			free(paquete);
		}

	}

}

//NO SE DEBE UTILIZAR MAS
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

	void *paquete_serializado = serializar_header(header);

	// Ciclo hasta que se envie toodo lo que quiero enviar
	if (socket >= 0) {
		while (bytes_enviados_totales < sizeof(t_header)) {
			if ((bytes_enviados = send(socket, paquete_serializado,
					sizeof(t_header), 0)) == -1) {
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

	// Ciclo hasta que se enviee toodo lo que quiero enviar
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

int conectar_servidor(char *ip, int puerto, void *funcion) {

	int socket_cliente;
	struct hostent *he;
	// Información del destino
	struct sockaddr_in addr_server;
	pthread_t hilo_recibir;

	if ((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		return -1;
	}

	// Abrimos el socket
	if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	// Configuración de dirección de servidor
	addr_server.sin_family = AF_INET; // Ordenación de bytes de la máquina
	addr_server.sin_port = htons(puerto); // short, Ordenación de bytes de la red
	addr_server.sin_addr = *((struct in_addr *) he->h_addr);
	memset(&(addr_server.sin_zero), 0, 8); // poner a cero el resto de la estructura

	// Conecto con servidor, si hay error finalizo
	if (connect(socket_cliente, (struct sockaddr *) &addr_server,
			sizeof(struct sockaddr)) == -1) {
		perror("connect");
		return -1;
	}

	// Creamos estructura para mandarsela a recv
	t_th_parametros_receive *param_receive = malloc(
			sizeof(t_th_parametros_receive));
	param_receive->socket_cliente = socket_cliente;
	param_receive->funcion = funcion;

	// Creamos el hilo para recibir mensajes desde el servidor
	pthread_create(&hilo_recibir, NULL, (void*) recibir_mensaje, param_receive);

	return socket_cliente;
}
