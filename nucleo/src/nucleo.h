/*
 * nucleo.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef NUCLEO_H_
#define NUCLEO_H_

// Valores por defecto de la configuracion

#define DEF_PUERTO_PROG 5000 // Puerto para recibir conexiones de los programas
#define DEF_PUERTO_CPU  5001 // Puerto para recibir conexiones de los cpus
#define DEF_QUANTUM  3 // Quantum del algoritmo Round Robin, se puede modificar en tiempo de ejecucion
#define DEF_QUANTUM_SLEEP 500 // Valor de retardo en milisegundos que el CPU debera esperar luego de ejecutar
//cada sentencia. Este valor puede modificarse en tiempo de ejecución
#define DEF_SEM_IDS ("SEM1", "SEM2", "SEM3")// Identificador de cada semaforo del sistema. Cada posicion del array representa un semaforo
#define DEF_SEM_INIT (0,0,5)// Valor inicial de cada semaforo definido en SEM_IDS, según su posicion
#define DEF_IO_ID ("Disco", "Impresora", "Scanner")
#define DEF_IO_SLEEP (1000, 2000,1000) // Retardo en milisegundos de cada unidad de operación de i/o de cada dispositivo definido en IO_IDS ​, segun su posicion
#define DEF_SHARED_VARS ("!Global", "!UnaVar", "!tiempo3")  // Identificador de cada variable compartida

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

#define CONSOLA 1
#define OK_CONSOLA 2
#define CPU 3
#define OK_CPU 4
#define UMC 5
#define OK_UMC 6

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
} t_config_nucleo;

void cargarConfiguracionNucleo(char *archivo, t_config_nucleo *configuracion);
int crear_servidor(t_configuracion_servidor *config_servidor);

#endif /* NUCLEO_H_ */
