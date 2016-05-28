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

// Funciones CPU - UMC
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22

// Funciones CPU - Nucleo
#define MENSAJE_OBTENER_VALOR_COMPARTIDA 1
#define MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA 2
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4
#define MENSAJE_ENTRADA_SALIDA 5
#define MENSAJE_WAIT 6
#define MENSAJE_SIGNAL 7
// Respuestas OK
#define RESPUESTA_OBTENER_VALOR_COMPARTIDA 11
#define RESPUESTA_ASIGNAR_VARIABLE_COMPARTIDA 12
#define RESPUESTA_IMPRIMIR 13
#define RESPUESTA_IMPRIMIR_TEXTO 14
#define RESPUESTA_ENTRADA_SALIDA 15
#define RESPUESTA_WAIT 16
#define RESPUESTA_SIGNAL 17
// Respuestas Error
#define ERROR_OBTENER_VALOR_COMPARTIDA 21
#define ERROR_ASIGNAR_VARIABLE_COMPARTIDA 22
#define ERROR_IMPRIMIR 23
#define ERROR_IMPRIMIR_TEXTO 24
#define ERROR_ENTRADA_SALIDA 25
#define ERROR_WAIT 26
#define ERROR_SIGNAL 27

typedef struct {
	char *ip_nucleo;
	int puerto_nucleo;
	char *ip_umc;
	int puerto_umc;
} t_config_cpu;

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion);

// Funciones CPU - UMC
void atender_umc(t_paquete *paquete, int socket_conexion);
void definir_variable(char *variable);
void obtener_posicion_variable(char * variable);
void handshake_cpu_umc();
void respuesta_handshake_cpu_umc(void *buffer);
void leer_pagina(int pagina, int offset, int tamanio);
void respuesta_leer_pagina(void *buffer);
void escribir_pagina(int pagina, int offset, int tamanio, int valor);

// Funciones CPU - Nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion);
void handshake_cpu_nucleo();
void obtener_valor_compartida(char *variable);
void asignar_valor_compartida(char *variable, int valor);
void imprimir_variable(char *variable, int valor);
void imprimir_texto(char *texto);
void entrada_salida(char *nombre, int tiempo);
void wait_semaforo(char *semaforo);
void signal_semaforo(char *semaforo);

#endif /* CPU_H_ */
