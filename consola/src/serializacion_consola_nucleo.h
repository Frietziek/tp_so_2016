/*
 * serializacion_consola_nucleo.h
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_CONSOLA_NUCLEO_H_
#define SERIALIZACION_CONSOLA_NUCLEO_H_

#include <serializacion.h>

// Funciones Consola - Nucleo
#define CODIGO 0
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4
#define MENSAJE_PROGRAMA_FINALIZADO 9
#define MENSAJE_MATAR_PROCESO 31
// Respuestas OK
#define RESPUESTA_IMPRIMIR 13
#define RESPUESTA_IMPRIMIR_TEXTO 14
#define RESPUESTA_PROGRAMA_FINALIZADO 19
#define RESPUESTA_MATAR_PROCESO 41
// Respuestas Error
#define ERROR_IMPRIMIR 23
#define ERROR_IMPRIMIR_TEXTO 24
#define ERROR_PROGRAMA_FINALIZADO 29
#define ERROR_MATAR_PROCESO 51

typedef struct {
	int valor;
} t_variable_valor;

typedef struct {
	char *texto;
} t_texto;

void deserializar_texto(void *buffer, t_texto *texto);
void deserializar_variable_valor(void *buffer, t_variable_valor *variable);
t_buffer *serializar_imprimir_texto(t_texto *texto);
void consola_nucleo(int socket_nucleo, int opcion);

#endif /* SERIALIZACION_CONSOLA_NUCLEO_H_ */
