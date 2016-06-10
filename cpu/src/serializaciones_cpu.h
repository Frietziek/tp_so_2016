/*
 * serializaciones_cpu.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef SERIALIZACIONES_CPU_H_
#define SERIALIZACIONES_CPU_H_

#include <serializacion.h>

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
} t_entrada_salida;
typedef struct {
	char *nombre;
} t_semaforo;
// Estructuras CPU - UMC
typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int socket_pedido;
} t_pagina;
typedef struct {
	int tamanio;
} t_pagina_tamanio;
typedef struct {
	int pagina;
	int offset;
	int tamanio;
	void *valor;
	int socket_pedido;
} t_pagina_completa;

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
// Funciones CPU - UMC
t_buffer *serializar_pagina(t_pagina *pagina);
void deserializar_pagina(void *buffer, t_pagina *pagina);
t_buffer *serializar_pagina_completa(t_pagina_completa *pagina);
void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina);
void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio);

#endif /* SERIALIZACIONES_CPU_H_ */
