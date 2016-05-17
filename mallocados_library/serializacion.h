/*
 * serializacion.h
 *
 *  Created on: 15/5/2016
 *      Author: utnso
 */

#ifndef MALLOCADOS_LIBRARY_SERIALIZACION_H_
#define MALLOCADOS_LIBRARY_SERIALIZACION_H_

typedef struct {
	int edad;
	char *nombre;
	int cp;
	char *apellido;
} t_persona;

typedef struct {
	void *contenido_buffer;
	int longitud_buffer;
} t_buffer;

void copiar_int_en_buffer(void *buffer, int una_cosa, int *posicion_buffer);

void copiar_char_en_buffer(void *buffer, char una_cosa, int *posicion_buffer);

void copiar_string_en_buffer(void *buffer, char *una_cosa, int *posicion_buffer);

t_buffer *serializar_persona(t_persona *persona);

void escribir_atributo_desde_int_de_buffer(void *buffer,
		int *atributo_de_estructura, int *posicion_buffer);

void escribir_atributo_desde_char_de_buffer(void *buffer,
		char *atributo_de_estructura, int *posicion_buffer);

//OJO, no esta el \0 en el buffer, lo ponemos nosotros en esta funcion
void escribir_atributo_desde_string_de_buffer(void *buffer,
		char **atributo_de_estructura, int *posicion_buffer);

void deserializar_persona(void* buffer, t_persona *persona);

#endif /* MALLOCADOS_LIBRARY_SERIALIZACION_H_ */
