/*
 * semaforos_nucleo.h
 *
 *  Created on: 29/5/2016
 *      Author: utnso
 */

#ifndef SEMAFOROS_NUCLEO_H_
#define SEMAFOROS_NUCLEO_H_

#include <semaphore.h>

sem_t mutex_pid_count;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_exec;
sem_t mutex_cola_block;
sem_t mutex_tabla_procesos;

#endif /* SEMAFOROS_NUCLEO_H_ */
