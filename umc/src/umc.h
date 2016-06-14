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
#define FLUSH 3
#define TLB 4
#define MEMORY 5

// Funciones globales de comunicacion
#define MENSAJE_HANDSHAKE 0
#define REPUESTA_HANDSHAKE 10
#define ERROR_HANDSHAKE 20

// Funciones de CPU - Swap
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
#define MENSAJE_INICIAR_PROGRAMA 3
#define MENSAJE_CAMBIO_PROCESO_ACTIVO 5
#define MENSAJE_LEER_PAGINA_PARA_ESCRIBIR 6
// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
#define RESPUESTA_INICIAR_PROGRAMA 13
#define RESPUESTA_FINALIZAR_PROGRAMA 14
#define RESPUESTA_CAMBIO_PROCESO_ACTIVO 15
#define RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR 16
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22
#define ERROR_INICIAR_PROGRAMA 23
#define ERROR_FINALIZAR_PROGRAMA 24
#define ERROR_CAMBIO_PROCESO_ACTIVO 25
#define ERROR_LEER_PAGINA_PARA_ESCRIBIR 26
// Funciones Nucleo - UMC
#define MENSAJE_INICIALIZAR_PROGRAMA 1
#define MENSAJE_MATAR_PROGRAMA 2
#define MENSAJE_FINALIZAR_PROGRAMA 4
// Respuestas OK
#define RESPUESTA_INICIALIZAR_PROGRAMA 11
#define RESPUESTA_MATAR_PROGRAMA 12

// Variables utilizadas
#define CANT_TABLAS_MAX 100 // numero suficientemente alto usado para reserver memoria, ver en todo caso otra mejor manera

typedef struct {
	int puerto;
	char *ip_swap;
	int puerto_swap;
	int marcos;
	int marco_size;
	int marco_x_proc;
	int entradas_tlb;
	int retardo;
	char * algoritmo;
} t_config_umc;

typedef struct {
	int pid;
	int pagina;
	int frame;
} t_tlb;

typedef struct {
	int numero_pagina;
	int pid;
	int frame;
	int presencia; // 0 -> no está en memoria / 1 -> si está en memoria
	int modificado;// 0 -> no está modificado / 1 -> si está modificado
	int uso; 		// usado por los algoritmos
} t_fila_tabla_pagina;

typedef struct{
	//int numero_pagina;
	//int pid;
	bool libre;
	int numero_marco;
	int direccion_mp;
} t_marco;

typedef struct {
	t_list * lista_paginas_mp; // lista de t_fila_tabla_pagina
	//int pid;
	int puntero;
} t_lista_algoritmo;

typedef struct {
	char * buffer;
	int tamanio_buffer;
	int pid;
	int pagina;
}t_buffer_escritura;

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

void iniciar_programa(void* buffer);

void respuesta_iniciar_programa(void *buffer,int id_mensaje);

void leer_pagina(void *buffer, int socket_conexion, t_config_umc *configuracion);

void respuesta_leer_pagina(void *buffer,int id_mensaje);

void enviar_pagina(int socket, int proceso_receptor, t_pagina_completa *pagina,int id_mensaje);

void escribir_pagina(void *buffer, int socket_conexion);

void finalizar_programa(void *buffer);

void handshake_umc_cpu(int socket_cpu, t_config_umc *configuracion);

void handshake_umc_nucleo(int socket_nucleo, t_config_umc *configuracion);

void handshake_proceso(int socket, t_config_umc *configuracion,
		int proceso_receptor, int id_mensaje);

int pagina_en_tlb();

void cambiar_retardo(t_config_umc *configuracion);

void generar_dump();

int buscar_pagina_tlb(int id_programa,int pagina);

t_marco * buscar_pagina_mp(int id_programa,int pagina);

void crear_listas();

void crear_marcos();

int retornar_direccion_mp(int marco);

void inicializar_pagina_cpu(t_pagina_completa * pagina_cpu,t_pagina * una_pagina, int socket_conexion);

void flush_tlb();

void flush_programa_tlb(int pid);

int guardar_en_mp(t_pagina_completa *pagina);

int cant_pag_x_proc(int pid);

int obtener_marco();

void guardar_en_TLB(t_pagina_completa * pagina,int marco);

void LRU(t_tlb * pagina_a_ubicar);

int reemplazar_pagina(t_fila_tabla_pagina * pagina_a_ubicar);

void marcar_modificada(int pid,int pagina);

void poner_en_buffer(t_pagina_completa * pagina);

void copiar_pagina_escritura_desde_buffer(int pid, int pagina, t_pagina_completa * pag_completa);

void mandar_a_swap(t_fila_tabla_pagina * pagina_a_swappear);

void armar_pagina_completa(t_fila_tabla_pagina * pagina_a_swappear,t_pagina_completa * pagina_completa);

void respuesta_leer_pagina_para_escribir(void *buffer, int id_mensaje);

void respuesta_escribir_pagina(void *buffer,int id_mensaje);

void liberar_marcos(int pid);

void respuesta_finalizar_programa(void *buffer,int id_mensaje);

void test();

#endif /* SRC_UMC_H_ */
