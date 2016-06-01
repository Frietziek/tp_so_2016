/*
 * semaforo_cpu.h
 *
 *  Created on: 28/5/2016
 *      Author: utnso
 */

#ifndef SEMAFORO_CPU_H_
#define SEMAFORO_CPU_H_

#include <semaphore.h>

sem_t s_pagina;
sem_t s_cpu_finaliza;
void *valor_pagina;


#endif /* SEMAFORO_CPU_H_ */
