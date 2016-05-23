/*
 * serializacion.h
 *
 *  Created on: 15/5/2016
 *      Author: utnso
 */

#ifndef MALLOCADOS_LIBRARY_SERIALIZACION_H_
#define MALLOCADOS_LIBRARY_SERIALIZACION_H_

#define PROCESO_CONSOLA = 0
#define PROCESO_NUCLEO = 1
#define PROCESO_CPU = 2
#define PROCESO_UMC = 3
#define PROCESO_SWAP = 4

typedef struct {
	int materias_aprobadas;
	int edad;
	char *nombre;
	int cp;
	char *apellido;
} t_persona;

typedef struct {
	int id_proceso_emisor;
	int id_proceso_receptor;
	int id_mensaje;
	int longitud_mensaje;
} t_header;

typedef struct {
	void *contenido_buffer;
	int longitud_buffer;
} t_buffer;

typedef struct {
	t_header *header;
	void *payload;
} t_paquete;

void copiar_int_en_buffer(void *buffer, int una_cosa, int *posicion_buffer);

void copiar_char_en_buffer(void *buffer, char una_cosa, int *posicion_buffer);

void copiar_string_en_buffer(void *buffer, char *una_cosa,
		int *posicion_buffer);

void copiar_void_en_buffer(void *buffer, t_buffer *t_buffer,
		int *posicion_buffer);

void escribir_atributo_desde_int_de_buffer(void *buffer,
		int *atributo_de_estructura, int *posicion_buffer);

void escribir_atributo_desde_char_de_buffer(void *buffer,
		char *atributo_de_estructura, int *posicion_buffer);

//OJO, no esta el \0 en el buffer, lo ponemos nosotros en esta funcion
void escribir_atributo_desde_string_de_buffer(void *buffer,
		char **atributo_de_estructura, int *posicion_buffer);

void escribir_atributo_desde_void_de_buffer(void *buffer,
		void **atributo_de_estructura, int *posicion_buffer);

t_buffer *serializar_persona(t_persona *persona);

void *serializar_header(t_header *header);

void *serializar_con_header(t_header *header, t_buffer *payload);

void deserializar_persona(void* buffer, t_persona *persona);

void deserializar_header(void* buffer, t_header *header);

//Devuelve un paquete con t_header y un void* que representa al payload.
t_paquete *deserializar_con_header(void *buffer);

#endif /* MALLOCADOS_LIBRARY_SERIALIZACION_H_ */
