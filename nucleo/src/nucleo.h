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
	t_list *variables;	//t_variables_stack
	t_list *argumentos; //t_posicion_memoria
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
	char *nombre_dispositivo;
	int retardo;
	t_queue *solicitudes; //t_solicitud_entrada_salida_cpu
} t_solicitudes_entrada_salida;

typedef struct {
	char *nombre_semaforo;
	int valor;
	t_queue *solicitudes;      //t_solicitud_semaforo
} t_solicitudes_semaforo;

typedef struct {
	int socket_cpu;
	int operacion; //siempre va a sumar operacion a valor, si es 1 suma 1 si es -1 resta 1
} t_solicitud_semaforo_cpu;

typedef struct {
	int socket_cpu;
	int cantidad_operaciones;
} t_solicitud_entrada_salida_cpu;

typedef struct {
	t_pcb *pcb;
	int socket_consola;
	int socket_cpu;
} t_fila_tabla_procesos;

void inicializar_variables_compartidas(char **shared_vars);

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

t_pcb *crear_PCB(char *codigo_de_consola);

void terminar_ejecucion(t_pcb *pcb);

int buscar_socket_cpu_por_pcb(t_pcb *pcb_a_finalizar);

void inicializar_colas_entrada_salida(char **io_ids, char **io_sleep);

void inicializar_solicitudes_semaforo(char **sem_id, char**sem_init);

void handshake(int socket, int proceso_receptor, int id_mensaje);

#endif /* NUCLEO_H_ *///
