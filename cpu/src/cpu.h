/*
 * cpu.h
 *
 *  Created on: 26/4/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <serializacion.h>

// Funciones globales de comunicacion
#define MENSAJE_HANDSHAKE 0
#define RESPUESTA_HANDSHAKE 10
#define ERROR_HANDSHAKE 20

// Funciones CPU - UMC
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22

typedef struct {
	char *ip_nucleo;
	int puerto_nucleo;
	char *ip_umc;
	int puerto_umc;
} t_config_cpu;

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion);

void inicio_variables_cpu();
int conecto_con_nucleo(t_config_cpu* configuracion);
int conecto_con_umc(t_config_cpu* configuracion);

// Funciones CPU - UMC
void atender_seniales(int signum);
void atender_umc(t_paquete *paquete, int socket_conexion);
void handshake_cpu_umc(int socket_servidor);
void respuesta_handshake_cpu_umc(void *buffer);
void respuesta_leer_pagina(void *buffer);

// Funciones CPU - Nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion);
void handshake_cpu_nucleo(int socket_servidor);

#endif /* CPU_H_ */
