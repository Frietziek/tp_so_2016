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

typedef struct {
	int indice[50][2];
} t_indice_codigo;

typedef struct {
// TODO Terminarlo
} t_indice_etiquetas;

typedef struct {
	int pagina;
	int offset;
	int size;
} t_posicion_memoria;

typedef struct {
	char *nombre_id;
	t_posicion_memoria posicion;
} t_variable_stack;

typedef struct {
	t_posicion_memoria *args;
	t_variable_stack *vars;
	int retPos;
	int retVar;
} t_indice_stack;

typedef struct {
	int pid;
	int pc;
	int paginas_codigo;
	t_indice_codigo *indice_codigo;
	t_indice_etiquetas *indice_etiquetas;
	t_indice_stack *indice_stack[100];
} t_pcb_completo;

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion);
void inicio_variables_cpu();
int conecto_con_nucleo(t_config_cpu* configuracion);
int conecto_con_umc(t_config_cpu* configuracion);
void atender_seniales(int signum);

// Funciones CPU - UMC
void atender_umc(t_paquete *paquete, int socket_conexion);
void handshake_cpu_umc(int socket_servidor);
void respuesta_handshake_cpu_umc(void *buffer);
void respuesta_leer_pagina(void *buffer);

// Funciones CPU - Nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion);
void handshake_cpu_nucleo(int socket_servidor);

#endif /* CPU_H_ */
