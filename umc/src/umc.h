/*
 * umc.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_

// Funciones de la consola de UMC
#define RETARDO 1
#define DUMP 2
#define TLB 3

// Funciones globales de comunicacion
#define MENSAJE_HANDSHAKE 0
#define MENSAJE_RESPUESTA_OK 10
#define MENSAJE_RESPUESTA_ERROR 11

// Funciones CPU - Swap
#define MENSAJE_INICIALIZAR_PROGRAMA 1
#define MENSAJE_LEER_PAGINA 2
#define MENSAJE_ESCRIBIR_PAGINA 3
#define MENSAJE_FINALIZAR_PROGRAMA 4

// Funciones CPU - UMC
#define MENSAJE_DEREFENCIAR 1
#define MENSAJE_ASIGNAR_VARIABLE 2

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

void atender_peticiones(t_config_umc *config, t_paquete *paquete);

void atender_cpu(t_config_umc *config, t_paquete *paquete);

void atender_nucleo(t_config_umc *config, t_paquete *paquete);

// Funciones UMC - Swap
void handshake_umc_swap();

void inicializar_programa();

void leer_pagina();

void escribir_pagina();

void finalizar_programa();

void cambiar_retardo();

void generar_dump();

void limpiar_contenido();

#endif /* SRC_UMC_H_ */
