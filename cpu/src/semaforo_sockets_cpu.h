/*
 * semaforo_sockets_cpu.h
 *
 *  Created on: 28/5/2016
 *      Author: utnso
 */

#ifndef SEMAFORO_SOCKETS_CPU_H_
#define SEMAFORO_SOCKETS_CPU_H_

#include <semaphore.h>
#include <commons/log.h> // Libreria de Logs
#include <commons/collections/list.h> // Libreria para listas
#include <parser/metadata_program.h>

typedef struct {
	int pagina;
	int offset;
	int size;
} t_posicion_memoria;

typedef struct {
	int id;
	t_posicion_memoria *posicion_memoria;
} t_variables_stack;

typedef struct {
	int posicion_retorno;
	t_posicion_memoria *posicion_variable_retorno;
	t_variables_stack **variables;
	t_posicion_memoria **argumentos;
} t_indice_stack;

typedef struct {
	int pid;
	int pc;
	int cant_paginas_codigo_stack;
	int estado;
	int stack_size_maximo;
	int stack_position;
	//t_size etiquetas_size; // Tamaño del mapa serializado de etiquetas
	char* etiquetas;
	t_size instrucciones_size;
	t_intructions **instrucciones_serializadas;
	int stack_size_actual;
	t_indice_stack **indice_stack;
} t_pcb;

typedef struct {
	int quantum;
	t_pcb *pcb;
} t_pcb_quantum;

// Semaforos para continuar proceso y finalizar
sem_t s_codigo; // Para cuando pido lectura de codigo
sem_t s_instruccion_finalizada; // Para esperar a recibir respuesta del UMC o Nucleo
sem_t s_variable_stack; // Para cuando pido lectura de variable
sem_t s_variable_compartida; // Para cuando pido lectura de var compartida
sem_t s_cpu_finaliza; // Cuando llega senial de finalizar cpu
void *valor_pagina;
int size_pagina;
int pagina_es_codigo; // 1 para codigo, 0 para valor int

// Sockets de los procesos a los cuales me conecto
int socket_nucleo;
int socket_umc;

// Tamanio de pagina de UMC
int tamanio_pagina;

// Logger
t_log *logger_manager;

// PCB - Quantum
t_pcb_quantum *pcb_quantum;

#endif /* SEMAFORO_SOCKETS_CPU_H_ */
