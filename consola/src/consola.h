/*
 * consola.h
 *
 *  Created on: 24/4/2016
 *      Author: utnso
 */

#ifndef SRC_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <comunicaciones.h>
#include <serializacion.h>
#include <signal.h>
#include "serializacion_consola_nucleo.h"

#define SRC_CONSOLA_H_

// Valores por defecto de la configuracion
#define DEF_PUERTO_Nucleo 3605 // Puerto para recibir conexiones (socket host)
#define DEF_IP_Nucleo "10.0.0.100" // Ip del proceso Nucleo (socket Server)


typedef struct {
	int puerto;
	char *ip;
} t_config_consola;

void cargaConfiguracionConsola(char *archivo, t_config_consola *configuracion);
void handshake_consola_nucleo(int socket_nucleo);
int Generar_Buffer_Programa(FILE *archivo,char **buffer);
void sig_handler(void);
void enviar_codigo (FILE *, int);
void atender_nucleo(t_paquete *paquete, int socket_conexion);

#endif /* SRC_CONSOLA_H_ */
