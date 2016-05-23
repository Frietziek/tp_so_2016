/*
 * serializacion_cpu_nucleo.h
 *
 *  Created on: 20/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_CPU_NUCLEO_H_
#define SERIALIZACION_CPU_NUCLEO_H_

typedef struct {
	char *nombre;
} t_variable;

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

t_buffer *serializar_variable_compartida(t_variable *variable);
void deserializar_variable_compartida(void *buffer,
		t_variable *variable);

t_buffer *serializar_asignar_variable_compartida(t_variable_completa *variable);
void deserializar_asignar_variable_compartida(void *buffer,
		t_variable_completa *variable);

t_buffer *serializar_imprimir_variable(t_variable_completa *variable);
void deserializar_imprimir_variable(void *buffer, t_variable_completa *variable);

t_buffer *serializar_imprimir_texto(t_texto *texto);
void deserializar_imprimir_texto(void *buffer, t_texto *texto);

t_buffer *serializar_entrada_salida(t_entrada_salida *entrada_salida);
void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida);

t_buffer *serializar_semaforo(t_semaforo *semaforo);
void deserializar_semaforo(void *buffer, t_semaforo *entrada_salida);

#endif /* SERIALIZACION_CPU_NUCLEO_H_ */
