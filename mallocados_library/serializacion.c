/*
 * serializacion.c
 *
 *  Created on: 15/5/2016
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <commons/string.h>
#include "serializacion.h"

void copiar_int_en_buffer(void *buffer, int una_cosa, int *posicion_buffer) {
	memcpy(buffer + *posicion_buffer, &una_cosa, sizeof(una_cosa));
	*posicion_buffer += sizeof(int);
}

void copiar_char_en_buffer(void *buffer, char una_cosa, int *posicion_buffer) {
	memcpy(buffer + *posicion_buffer, &una_cosa, sizeof(una_cosa));
	*posicion_buffer += sizeof(char);
}

//Antes de poner el char* en el buffer primero pone un int con la longitud
void copiar_string_en_buffer(void *buffer, char *una_cosa, int *posicion_buffer) {
	copiar_int_en_buffer(buffer, strlen(una_cosa), posicion_buffer);
	memcpy(buffer + *posicion_buffer, una_cosa, strlen(una_cosa));
	*posicion_buffer += strlen(una_cosa);
}

//Antes de poner el void* en el buffer primero pone un int con la longitud
void copiar_void_en_buffer(void *buffer, t_buffer *t_buffer,
		int *posicion_buffer) {
	copiar_int_en_buffer(buffer, t_buffer->longitud_buffer, posicion_buffer);
	memcpy(buffer + *posicion_buffer, t_buffer->contenido_buffer,
			t_buffer->longitud_buffer);
	*posicion_buffer += t_buffer->longitud_buffer;
}

void escribir_atributo_desde_int_de_buffer(void *buffer,
		int *atributo_de_estructura, int *posicion_buffer) {
	memcpy(atributo_de_estructura, buffer + *posicion_buffer, sizeof(int));
	*posicion_buffer += sizeof(int);
}

void escribir_atributo_desde_char_de_buffer(void *buffer,
		char *atributo_de_estructura, int *posicion_buffer) {
	memcpy(atributo_de_estructura, buffer + *posicion_buffer, sizeof(char));
	*posicion_buffer += sizeof(char);
}

void escribir_atributo_desde_string_de_buffer(void *buffer,
		char **atributo_de_estructura, int *posicion_buffer) {

	int longitud_de_atributo;
	memcpy(&longitud_de_atributo, buffer + *posicion_buffer, sizeof(int));

	*posicion_buffer += sizeof(int);

	*atributo_de_estructura = malloc(longitud_de_atributo + 1);

	memcpy(*atributo_de_estructura, buffer + *posicion_buffer,
			longitud_de_atributo);

	(*atributo_de_estructura)[longitud_de_atributo] = '\0';

	*posicion_buffer += longitud_de_atributo;

}

void escribir_atributo_desde_void_de_buffer(void *buffer,
		void **atributo_de_estructura, int *posicion_buffer) {

	int longitud_de_atributo;
	memcpy(&longitud_de_atributo, buffer + *posicion_buffer, sizeof(int));

	*posicion_buffer += sizeof(int);

	*atributo_de_estructura = malloc(longitud_de_atributo);

	memcpy(*atributo_de_estructura, buffer + *posicion_buffer,
			longitud_de_atributo);

	*posicion_buffer += longitud_de_atributo;

}

//Despues hay que hacer un free de lo que va a retornar
t_buffer *serializar_header(t_header *header) {

	int cantidad_a_reservar = sizeof(header->id_proceso_emisor)
			+ sizeof(header->id_proceso_receptor) + sizeof(header->id_mensaje)
			+ sizeof(header->longitud_mensaje);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, header->id_proceso_emisor, &posicion_buffer);

	copiar_int_en_buffer(buffer, header->id_proceso_receptor, &posicion_buffer);

	copiar_int_en_buffer(buffer, header->id_mensaje, &posicion_buffer);

	copiar_int_en_buffer(buffer, header->longitud_mensaje, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	free(buffer);
	return (estructura_buffer);
}

//El payload es el resultado de una funcion serializar
void *serializar_con_header(t_header *header, t_buffer *payload) {

	int cantidad_a_reservar = sizeof(t_header) + payload->longitud_buffer;

	void *buffer = malloc(cantidad_a_reservar);

	t_buffer *header_de_mensaje = serializar_header(header);

	int posicion_buffer = 0;

	memcpy(buffer, header_de_mensaje->contenido_buffer,
			header_de_mensaje->longitud_buffer);
	posicion_buffer += header_de_mensaje->longitud_buffer;

	memcpy(buffer + posicion_buffer, payload->contenido_buffer,
			payload->longitud_buffer);

	posicion_buffer += payload->longitud_buffer;

	free(header_de_mensaje);
	return buffer;

}

void deserializar_header(void *buffer, t_header *header) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(header->id_proceso_emisor),
			&posicion_buffer);

	escribir_atributo_desde_int_de_buffer(buffer,
			&(header->id_proceso_receptor), &posicion_buffer);

	escribir_atributo_desde_int_de_buffer(buffer, &(header->id_mensaje),
			&posicion_buffer);

	escribir_atributo_desde_int_de_buffer(buffer, &(header->longitud_mensaje),
			&posicion_buffer);
}

t_paquete *deserializar_con_header(void *buffer) {

	t_header *header = malloc(sizeof(t_header));

	deserializar_header(buffer, header);

	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->header = header;

	if (header->longitud_mensaje > 0) {

		paquete->payload = malloc(header->longitud_mensaje);

		memcpy(paquete->payload, buffer + sizeof(t_header),
				header->longitud_mensaje);
	}
	free(header);

	return paquete;

}

//Despues hay que hacer un free de lo que va a retornar
t_buffer *serializar_persona(t_persona *persona) {

	int cantidad_a_reservar = sizeof(persona->cp) + sizeof(persona->edad)
			+ sizeof(int) + strlen(persona->apellido) + sizeof(int)
			+ strlen(persona->nombre) + sizeof(persona->materias_aprobadas);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, persona->materias_aprobadas, &posicion_buffer);

	copiar_string_en_buffer(buffer, persona->nombre, &posicion_buffer);

	copiar_int_en_buffer(buffer, persona->edad, &posicion_buffer);

	copiar_int_en_buffer(buffer, persona->cp, &posicion_buffer);
	copiar_string_en_buffer(buffer, persona->apellido, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	free(buffer);
	return (estructura_buffer);
}

void deserializar_persona(void *buffer, t_persona *persona) {

	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer,
			&(persona->materias_aprobadas), &posicion_buffer);

	escribir_atributo_desde_string_de_buffer(buffer, &(persona->nombre),
			&posicion_buffer);

	escribir_atributo_desde_int_de_buffer(buffer, &(persona->edad),
			&posicion_buffer);

	escribir_atributo_desde_int_de_buffer(buffer, &(persona->cp),
			&posicion_buffer);

	escribir_atributo_desde_string_de_buffer(buffer, &(persona->apellido),
			&posicion_buffer);

}

//main ejemplo
//
//t_persona *persona = malloc(sizeof(t_persona));
//	persona->edad = 22;
//	persona->cp = 1414;
//	persona->nombre = "santi";
//	persona->apellido = "bbb";
//
//	void *buffer = serializar_persona(persona);
//
//	t_persona *persona2 = malloc(sizeof(t_persona));
//
//	deserializar_persona(buffer, persona2);
//
//	printf("%s", persona2->apellido);
//
//	free(buffer);
//	free(persona);
//	free(persona2);

