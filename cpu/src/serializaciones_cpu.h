/*
 * serializaciones_cpu.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef SERIALIZACIONES_CPU_H_
#define SERIALIZACIONES_CPU_H_

#include <serializacion.h>
#include <parser/metadata_program.h>

// Estructuras CPU - Nucleo
typedef struct {
	char *nombre;
} t_variable;
typedef struct {
	int valor;
} t_variable_valor;
typedef struct {
	char *nombre;
	int valor;
} t_variable_completa;
typedef struct {
	char *texto;
} t_texto;
typedef struct {
	char *nombre_dispositivo;
	int tiempo;
	int pid;
} t_entrada_salida;
typedef struct {
	char *nombre;
} t_semaforo;
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
	int cantidad_argumentos;
	t_posicion_memoria *argumentos;
} t_indice_stack;
typedef struct {
	int pid;
	int pc;
	int cant_paginas_codigo_stack;
	int estado;
	int contexto_actual;
	int stack_size_fisico;
	int stack_position;
	int stack_pointer;
	t_size etiquetas_size; // Tamaño del mapa serializado de etiquetas
	char* etiquetas;
	t_size instrucciones_size;
	t_intructions *instrucciones_serializadas;
	int stack_size;
	t_indice_stack *indice_stack;
} t_pcb;
typedef struct {
	int quantum;
	t_pcb *pcb;
} t_pcb_quantum;
// Estructuras CPU - UMC
typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_pagina_pedido;
typedef struct {
	int tamanio;
} t_pagina_tamanio;
typedef struct {
	int pagina;
	int offset;
	int tamanio;
	void *valor;
} t_pagina_pedido_completa;
typedef struct {
	int id_programa;
} t_programa;

// Funciones CPU - Nucleo
t_buffer *serializar_variable(t_variable *variable);
void deserializar_variable(void *buffer, t_variable *variable);
t_buffer *serializar_variable_completa(t_variable_completa *variable);
void deserializar_variable_completa(void *buffer, t_variable_completa *variable);
t_buffer *serializar_variable_valor(t_variable_valor *variable);
void deserializar_variable_valor(void *buffer, t_variable_valor*variable);
t_buffer *serializar_texto(t_texto *texto);
void deserializar_texto(void *buffer, t_texto *texto);
t_buffer *serializar_entrada_salida(t_entrada_salida *entrada_salida);
void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida);
t_buffer *serializar_semaforo(t_semaforo *semaforo);
void deserializar_semaforo(void *buffer, t_semaforo *entrada_salida);
t_buffer *serializar_pcb_quantum(t_pcb_quantum *pcb_quantum);
void deserializar_pcb_quantum(void *buffer, t_pcb_quantum *pcb_quantum);
// Funciones CPU - UMC
t_buffer *serializar_programa(t_programa *programa);
void deserializar_programa(void *buffer, t_programa *programa);
t_buffer *serializar_pagina_pedido(t_pagina_pedido *pagina);
void deserializar_pagina_pedido(void *buffer, t_pagina_pedido *pagina);
t_buffer *serializar_pagina_pedido_completa(t_pagina_pedido_completa *pagina);
void deserializar_pagina_pedido_completa(void *buffer, t_pagina_pedido_completa *pagina);
void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio);

#endif /* SERIALIZACIONES_CPU_H_ */
