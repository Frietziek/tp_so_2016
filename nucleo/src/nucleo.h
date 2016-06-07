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
#include <parser/metadata_program.h>

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

#define NO_ASIGNADO -10

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
	int cant_paginas_codigo_stack;
	int estado;
	t_intructions* instrucciones_serializadas;
	t_size instrucciones_size;
	t_size etiquetas_size;			// Tamaño del mapa serializado de etiquetas
	char* etiquetas;
	int stack_size;
	int stack_position;
} t_pcb;

typedef struct {
	char *nombre_dispositivo;
	int retardo;
	t_queue *solicitudes;
} t_solicitudes_entrada_salida;

typedef struct {
	int socket_cpu;
	int cantidad_operaciones;
} t_solicitud_entrada_salida_cpu;

typedef struct { //TODO llenarme con parser metadata
	t_pcb *pcb;
	int socket_consola;
	int socket_cpu;
} t_fila_tabla_procesos;

void cargar_configuracion_nucleo(char *archivo, t_config_nucleo *configuracion);

t_pcb *buscar_pcb_por_socket_consola(int _socket_consola);

void pedir_pagina_tamanio(int socket_umc);

void atender_umc(t_paquete *paquete, int socket_conexion);

void atender_consola(t_paquete *paquete_buffer, int socket_consola);

t_pcb *buscar_pcb_por_socket_consola(int socket_consola);

void enviar_programa_completo_a_umc(int pid, int cant_paginas_codigo_stack,
		char *codigo);

void agregar_pcb_a_tabla_procesos(t_pcb *pcb, int socket_consola);

int obtener_cantidad_paginas_codigo_stack(char *codigo_de_consola);

t_pcb *crearPCB(char *codigo_de_consola);

void terminar_ejecucion(t_pcb *pcb);

int buscar_socket_cpu_por_pcb(t_pcb *pcb_a_finalizar);

void inicializar_colas_entrada_salida(char **io_ids, char **io_sleep);

void handshake(int socket, int proceso_receptor, int id_mensaje);

#endif /* NUCLEO_H_ *///
