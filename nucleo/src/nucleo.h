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
#include <commons/log.h>
#include <parser/metadata_program.h>
#include <semaphore.h>
#include <sys/inotify.h>
#include <errno.h>
#include <sys/types.h>
#include "serializacion_nucleo_umc.h"

#define NEW 0
#define READY 1
#define EXEC  2
#define BLOCK 3
#define EXIT 4

#define NO_ASIGNADO 90 //estaba negativo y pinchaba al asignarse en un int, de ultima cambiar el tipo
#define VALOR_INICIAL_VARIABLE_COMPARTIDA 0

#define PATH_CONFIGURACIONES_NUCLEO "config.nucleo.ini"

//------------ Constantes para ionotify-----------------//
#define IONOTIFY_EVENT_SIZE (sizeof (struct inotify_event) + 30)
#define IONOTIFY_BUFFER_EVENT_SIZE (30 * IONOTIFY_EVENT_SIZE)
//-----------------------------------------------------//

t_log *logger_manager;

typedef struct {
	int pagina;
	int offset;
	int size;
} t_posicion_memoria;

typedef struct {
	char id;
	t_posicion_memoria *posicion_memoria;
} t_variables_stack;

typedef struct {
	int posicion_retorno;
	t_posicion_memoria *posicion_variable_retorno;
	int cantidad_variables;
	t_variables_stack *variables;
} t_indice_stack;

typedef struct {
	int pid;
	int pc;
	int cant_paginas_codigo_stack;
	int estado;
	int contexto_actual;
	int stack_size_fisico; // Stack size de los registros que ocupan en mem fisica
	int stack_position;
	int stack_pointer;
	t_size etiquetas_size; // Tamaño del mapa serializado de etiquetas
	char* etiquetas;
	t_size instrucciones_size;
	t_intructions *instrucciones_serializadas;
	int stack_size; // Stack size de la estructura logica
	t_indice_stack *indice_stack;
} t_pcb;

typedef struct {
	int quantum;
	t_pcb *pcb;
} t_pcb_quantum;

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
	int retardo;
	int posicion_array_semaforo;
	t_queue *solicitudes; //t_solicitud_entrada_salida_cpu
} t_solicitudes_entrada_salida;

typedef struct {
	int socket_cpu;
	int cantidad_operaciones;
	int pid;
} t_solicitud_entrada_salida_cpu;

typedef struct {
	int valor;
	int posicion_semaforo_contador_solicitudes;
	t_queue *solicitudes;      //contiene (int) pid de proceso
} t_atributos_semaforo;

typedef struct {
	t_pcb *pcb;
	int socket_consola;
	int socket_cpu;
} t_fila_tabla_procesos;

typedef struct {
	int valor;
} t_valor_variable_compartida;

typedef struct {
	int socket;
} t_valor_socket_cola_semaforos;

typedef struct {
	int socket_cpu;
} t_cpu;

void inicializar_variables_compartidas(char **shared_vars);

void cargar_configuracion_nucleo(char *archivo, t_config_nucleo *configuracion);

void pedir_pagina_tamanio(int socket_umc);

void atender_umc(t_paquete *paquete, int socket_conexion);

void atender_consola(t_paquete *paquete_buffer, int socket_consola);

t_pcb *buscar_pcb_por_socket_consola(int socket_consola);

int buscar_socket_consola_por_socket_cpu(int socket_cpu);

void enviar_programa_completo_a_umc(int pid, int cant_paginas_codigo_stack,
		char *codigo);

void libero_pcb(t_pcb *pcb);

void agregar_pcb_a_lista_procesos(t_pcb *pcb, int socket_consola);

int obtener_cantidad_paginas_codigo_stack(char *codigo_de_consola);

t_pcb *crear_PCB(char *codigo_de_consola);

void matar_ejecucion(t_pcb *pcb);

int buscar_socket_cpu_por_pcb(t_pcb *pcb_a_finalizar);

void inicializar_colas_entrada_salida(char **io_ids, char **io_sleep);

void atender_solicitudes_entrada_salida(t_solicitudes_entrada_salida *io);

void inicializar_solicitudes_semaforo(char **sem_id, char**sem_init);

void enviar_header_completado(int socket, int proceso_receptor, int id_mensaje);
void envio_buffer_a_proceso(int socket_proceso, int proceso_receptor,
		int id_mensaje, char* mensaje_fallo_envio, t_buffer *buffer);

// Funciones del nucleo que hay que desarrollar
int obtener_variable_compartida(char *nombre_variable_compartida);
void asignar_variable_compartida(char *nombre_variable_compartida, int valor);
int devuelve_socket_consola(int socket_cpu);
void bloquear_pcb_semaforo(char *nombre_semaforo, int socket_cpu);
int obtener_valor_semaforo(char *semaforo_nombre);
void asignar_pcb_a_cpu(int socket_cpu);
int wait_semaforo(char *semaforo_nombre);
int signal_semaforo(char *semaforo_nombre);
t_pcb *buscar_pcb_por_pid(int pid);
void sacar_socket_cpu_de_tabla(int socket_cpu);
int buscar_socket_consola_por_pid(int pid);
void eliminar_proceso_de_lista_procesos_con_pid(int pid);
void atiendo_quantum(void *buffer, int socket_conexion);
void atiendo_programa_finalizado(void *buffer, int socket_cpu);
void actualizar_pcb_y_ponerlo_en_ready_con_socket_cpu(t_pcb *pcb,
		int socket_cpu);
void finalizar_proceso_en_lista_proc_con_socket_cpu(t_pcb * pcb, int socket_cpu);
void * queue_pop_pid(t_queue *self, int pid);
void agregar_cpu_disponible(int socket_conexion);
void asignar_pcb_a_cola_exec();
void actualizar_pcb_y_ponerlo_en_exec_con_socket_cpu(t_pcb *pcb, int socket_cpu);
void respuesta_matar(void * buffer, int socket_cpu);
void desbloquear_pcb_semaforo(t_atributos_semaforo *atributos);
void avisar_para_que_desbloquee(char *nombre_sem);
void monitorear_configuraciones();
void cargar_nuevas_configuraciones_del_nucleo(char *archivo_configuracion,
		t_config_nucleo *configuracion_nucleo);
void actualizar_estado_pcb(t_pcb *pcb, int estado);
void saco_socket_cpu(t_pcb *pcb);
t_fila_tabla_procesos * buscar_fila_por_socket_cpu( socket_cpu);
#endif /* NUCLEO_H_ *///
