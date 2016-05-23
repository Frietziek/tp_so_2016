/*
 * nucleo.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef NUCLEO_H_
#define NUCLEO_H_

#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

// Valores de la configuracion

/* EJEMPLO
 DEF_PUERTO_PROG=5000
 DEF_PUERTO_CPU=5001
 DEF_QUANTUM=3
 DEF_QUANTUM_SLEEP=500
 DEF_IO=[Disco, Impresora, Scanner]
 DEF_IO_SLEEP=[1000, 2000, 1000]
 DEF_SEM_IDS=[SEM1, SEM2, SEM3]
 DEF_SEM_INIT= [0,0,5]
 DEF_SHARED_VARS= ["!Global", "!UnaVar", "!tiempo3"]
 DEF_STACK_SIZE=2
 * */

#define NEW 0
#define READY 1
#define EXEC  2
#define BLOCK 3
#define EXIT 4

typedef struct {
	int puerto_prog;
	int puerto_cpu;
	int quantum;
	int quantum_sleep;
	char **io_id;
	char **io_sleep;
	char **sem_id;
	char **sem_init;
	char **shared_vars;
	char *ip_umc;
	int puerto_umc;
	int stack_size;
} t_config_nucleo;

typedef struct { //TODO llenarme con parser metadata
	int pid;
	int pc;
	int cant_paginas_codigo;
} t_pcb;

void cargarConfiguracionNucleo(char *archivo, t_config_nucleo *configuracion);

void obtener_tamanio_pagina(int *tamanio_pagina, void *buffer);

int conectar_umc_y_obtener_tamanio_pagina(t_config_nucleo *configuracion);

void atender_cpu(t_config_nucleo*config, t_paquete *paquete);

void atender_consola(void *buffer);

t_pcb *crearPCB(char *codigo_de_consola);

void agregar_pcb_a_cola(t_queue *cola, t_pcb *pcb);

void terminar_ejecucion();

#endif /* NUCLEO_H_ *///
