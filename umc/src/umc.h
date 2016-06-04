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

// Funciones de CPU - Swap
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
#define MENSAJE_INICIAR_PROGRAMA 3
#define MENSAJE_FINALIZAR_PROGRAMA 4
#define MENSAJE_CAMBIO_PROCESO_ACTIVO 5
// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
#define RESPUESTA_INICIAR_PROGRAMA 13
#define RESPUESTA_FINALIZAR_PROGRAMA 14
#define RESPUESTA_CAMBIO_PROCESO_ACTIVO 5
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22
#define ERROR_INICIAR_PROGRAMA 23
#define ERROR_FINALIZAR_PROGRAMA 24
#define ERROR_CAMBIO_PROCESO_ACTIVO 5

// Funciones Nucleo
#define MENSAJE_INICIALIZAR_PROGRAMA 1
// Respuestas OK
#define RESPUESTA_INICIALIZAR_PROGRAMA 11
// Respuestas Error
#define ERROR_INICIALIZAR_PROGRAMA 21

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

typedef struct {
	int pagina;
	int frame;
} t_tlb;

typedef struct {
	int frame;
	char presencia;
	char modificado;
} t_fila_tabla_pagina;

void carga_configuracion_UMC(char *archivo, t_config_umc *configuracion);

t_configuracion_servidor* creo_servidor_umc(t_config_umc* configuracion);

int conecto_con_swap(t_config_umc* configuracion);

void menu_principal(t_config_umc *configuracion);

void atender_peticiones(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion);

void atender_cpu(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion);

void atender_nucleo(t_paquete *paquete, int socket_conexion,
		t_config_umc *configuracion);

void atender_swap(t_paquete *paquete, int socket_conexion);

void handshake_umc_swap(int socket_servidor, t_config_umc *configuracion);

void respuesta_handshake_umc_swap();

void iniciar_programa(void *buffer);

void respuesta_iniciar_programa(void *buffer);

void leer_pagina(void *buffer, int socket_conexion, t_config_umc *configuracion);

void respuesta_leer_pagina(void *bufferfff);

void enviar_pagina(int socket, int proceso_receptor, t_pagina_completa *pagina);

void escribir_pagina(void *buffer, int socket_conexion);

void finalizar_programa(void *buffer, int socket);

void handshake_umc_cpu(int socket_cpu, t_config_umc *configuracion);

void handshake_umc_nucleo(int socket_nucleo, t_config_umc *configuracion);

void handshake_proceso(int socket, t_config_umc *configuracion,
		int proceso_receptor, int id_mensaje);

int pagina_en_tlb();

void cambiar_retardo(t_config_umc *configuracion);

void generar_dump();

void limpiar_contenido();

#endif /* SRC_UMC_H_ */
