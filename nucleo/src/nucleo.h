/*
 * nucleo.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef NUCLEO_H_
#define NUCLEO_H_

#include <commons/collections/dictionary.h>

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
 * */

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
} t_config_nucleo;

void cargarConfiguracionNucleo(char *archivo, t_config_nucleo *configuracion);
void funcion_saludar(t_config_nucleo*config, void *buffer);

#endif /* NUCLEO_H_ *///
