/*
 * semaforo_sockets_cpu.h
 *
 *  Created on: 28/5/2016
 *      Author: utnso
 */

#ifndef SEMAFORO_SOCKETS_CPU_H_
#define SEMAFORO_SOCKETS_CPU_H_

#include <semaphore.h>

sem_t s_pagina;
sem_t s_cpu_finaliza;
void *valor_pagina;

// Sockets de los procesos a los cuales me conecto
int socket_nucleo;
int socket_umc;

#endif /* SEMAFORO_SOCKETS_CPU_H_ */
