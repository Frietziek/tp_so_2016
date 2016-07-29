/*
 * cpu.h
 *
 *  Created on: 26/4/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCES y otros
#include <pthread.h> // Para manejar la ejecucion de instrucciones
#include <unistd.h> // Funcion close
#include <commons/config.h> // Funciones para leer archivos ini
#include <semaphore.h> // Semaforos s_pagina y s_cpu_corriendo
#include <signal.h> // Signal sigusr1
#include <commons/log.h> // Libreria de Logs
#include <commons/collections/list.h> // Libreria para listas
#include <comunicaciones.h>
#include <serializacion.h>
#include "comunicaciones_cpu.h"
#include "serializaciones_cpu.h"
#include "funciones_comunes.h"
#include "primitivas_ansisop.h"
#include "mensajes_cpu.h"

typedef struct {
	char *ip_nucleo;
	int puerto_nucleo;
	char *ip_umc;
	int puerto_umc;
} t_config_cpu;

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion);
void inicio_seniales_semaforos();
void cierro_cpu(t_config_cpu* configuracion);
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
void recibo_PCB(void *buffer);
void enviar_PCB(int id_mensaje);
void cambio_proceso_activo(int id_programa);
void ejecuto_instrucciones();
char *devuelve_instruccion_a_ejecutar();
int devuelve_id_mensaje();
int calcula_paginas_instruccion();
t_intructions *deserializo_instruccion(int pc);
void envio_excepcion_nucleo(int id_excepcion, char *mensaje_excepcion);
void leo_instruccion_desde_UMC(int pagina);
void respuesta_leer_compartida(void *buffer);
void libero_pcb();
int calcula_tamanio_instruccion(t_intructions *instruccion,
		t_pagina_pedido *p_pagina, int pagina);

#endif /* CPU_H_ */
