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
	memcpy(buffer + *posicion_buffer, &una_cosa, sizeof(int));
	*posicion_buffer += sizeof(int);
}

void copiar_char_en_buffer(void *buffer, char una_cosa, int *posicion_buffer) {
	memcpy(buffer + *posicion_buffer, &una_cosa, sizeof(char));
	*posicion_buffer += sizeof(char);
}

void copiar_string_en_buffer(void *buffer, char *una_cosa, int *posicion_buffer) {
	//int longitud = strlen(una_cosa);
	copiar_int_en_buffer(buffer, strlen(una_cosa), posicion_buffer);
	memcpy(buffer + *posicion_buffer, una_cosa, strlen(una_cosa));
	*posicion_buffer += strlen(una_cosa);
}


//void *serializar_persona(t_persona *persona) {
//Despues hay que hacer un free de lo que va a retornar
//
//	int cantidad_a_reservar = sizeof(persona->cp) + sizeof(persona->edad)
//			+ sizeof(int) + strlen(persona->apellido) + sizeof(int)
//			+ strlen(persona->nombre);
//	void *buffer = malloc(cantidad_a_reservar);
//
//	int posicion_buffer = 0;
//
//	copiar_int_en_buffer(buffer, persona->edad, &posicion_buffer);
//
//	copiar_string_en_buffer(buffer, persona->nombre, &posicion_buffer);
//
//	copiar_int_en_buffer(buffer, persona->cp, &posicion_buffer);
//	copiar_string_en_buffer(buffer, persona->apellido, &posicion_buffer);
//
//	return (buffer);
//}

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

//void deserializar_persona(void *buffer, t_persona *persona) {
//
//	int posicion_buffer = 0;
//
//	escribir_atributo_desde_int_de_buffer(buffer, &(persona->edad),
//			&posicion_buffer);
//
//	escribir_atributo_desde_string_de_buffer(buffer, &(persona->nombre),
//			&posicion_buffer);
//
//	escribir_atributo_desde_int_de_buffer(buffer, &(persona->cp),
//			&posicion_buffer);
//
//	escribir_atributo_desde_string_de_buffer(buffer, &(persona->apellido),
//			&posicion_buffer);
//
//}


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


