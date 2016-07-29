/*
 * consola.h
 *
 *  Created on: 24/4/2016
 *      Author: utnso
 */

#ifndef SRC_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Funcion close
#include <semaphore.h>
#include <commons/config.h>
#include <commons/string.h>
#include <comunicaciones.h>
#include <serializacion.h>
#include <signal.h>
#include <commons/log.h>
#include <commons/string.h>
#include "serializacion_consola_nucleo.h"

#define SRC_CONSOLA_H_

// Valores por defecto de la configuracion
#define DEF_PUERTO_NUCLEO 3605 // Puerto para recibir conexiones (socket host)
#define DEF_IP_NUCLEO "10.0.0.100" // Ip del proceso Nucleo (socket Server)
#define DEF_NOMBRE_SCRIPT "undefined"

typedef struct {
	int puerto_nucleo;
	char *ip_nucleo;
	char *nombre_script;
} t_config_consola;

sem_t s_consola_finaliza; // Cuando llega senial de finalizar cpu

void cargar_configuracion_consola(char *archivo,
		t_config_consola *configuracion);
void handshake_consola_nucleo(int socket_nucleo);
//int Generar_Buffer_Programa(FILE *archivo,char **buffer); //TODO: Esta funcion no se esta utilizando, analizar despues si sirve
void sig_handler(void);
void enviar_codigo_al_nucleo(FILE *, int);
void atender_nucleo(t_paquete *paquete, int socket_conexion);
void avisar_nucleo_de_terminacion_programa();
void enviar_handshake_al_nucleo(int socket_nucleo);

#endif /* SRC_CONSOLA_H_ */
