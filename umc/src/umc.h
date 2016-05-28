/*
 * umc.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_

#include "serializacion_umc_swap.h"

// Funciones de la consola de UMC
#define RETARDO 1
#define DUMP 2
#define TLB 3

// Funciones globales de comunicacion
#define MENSAJE_HANDSHAKE 0
#define REPUESTA_HANDSHAKE 10
#define ERROR_HANDSHAKE 20

// Funciones CPU - Swap
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
#define MENSAJE_INICIAR_PROGRAMA 3
#define MENSAJE_FINALIZAR_PROGRAMA 4
// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
#define RESPUESTA_INICIALIZAR_PROGRAMA 13
#define RESPUESTA_FINALIZAR_PROGRAMA 14
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22
#define ERROR_INICIALIZAR_PROGRAMA 23
#define ERROR_FINALIZAR_PROGRAMA 24

typedef struct {
	int puerto;
	char *ip_swap;
	int puerto_swap;
	int marcos;
	int marco_size;
	int marco_x_proc;
	int entradas_tlb;
	int retardo;
} t_config_umc;

void carga_configuracion_UMC(char *archivo, t_config_umc *configuracion);

void atender_peticiones(t_paquete *paquete, int socket_conexion);

void atender_cpu(t_paquete *paquete, int socket_conexion);

void atender_nucleo(t_paquete *paquete, int socket_conexion);

void atender_swap(t_paquete *paquete, int socket_conexion);

void handshake_umc_swap();

void iniciar_programa(void *buffer, int socket);

void respuesta_iniciar_programa(void *buffer);

void leer_pagina(void *buffer, int socket_conexion);

void respuesta_leer_pagina(void *bufferfff);

void enviar_pagina(int socket, int proceso_receptor, t_pagina_completa *pagina);

void escribir_pagina(void *buffer, int socket_conexion);

void finalizar_programa(void *buffer, int socket);

void handshake_umc_cpu(int socket_cpu);

void handshake_umc_nucleo(int socket_nucleo);

void handshake_proceso(int socket, int proceso_receptor, int id_mensaje);

int pagina_en_memoria();

void cambiar_retardo();

void generar_dump();

void limpiar_contenido();

#endif /* SRC_UMC_H_ */
