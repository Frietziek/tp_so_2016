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
#include <unistd.h> // Funcion close
#include <commons/config.h> // Funciones para leer archivos ini
#include <semaphore.h> // Semaforos s_pagina y s_cpu_corriendo
#include <signal.h> // Signal sigusr1
#include <commons/log.h> // Libreria de Logs
#include <commons/collections/list.h> // Libreria para listas
#include <parser/metadata_program.h>
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

int obtener_cantidad_paginas_codigo_stack(char *codigo_de_consola);

t_pcb *crear_PCB(char *codigo_de_consola);

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
void cambio_proceso_activo();
void ejecuto_instrucciones();
t_intructions *deserializo_instruccion(int pc);
void envio_excepcion_nucleo(int id_excepcion, char *mensaje_excepcion);
void leo_instruccion_desde_UMC(t_pcb *pcb);

t_buffer *serializar_pcb_quantum(t_pcb_quantum *pcb_quantum);
void deserializar_pcb_quantum(void *buffer, t_pcb_quantum *pcb_quantum);

#endif /* CPU_H_ */
