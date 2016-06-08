/*
 * serializacion_nucleo_umc.h
 *
 *  Created on: 28/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_NUCLEO_UMC_H_
#define SERIALIZACION_NUCLEO_UMC_H_

// UMC - Nucleo OK
#define RESPUESTA_INICIALIZAR_PROGRAMA 11
#define RESPUESTA_MATAR_PROGRAMA 12
// UMC - Nucleo Error
#define ERROR_INICIALIZAR_PROGRAMA 21
#define ERROR_MATAR_PROGRAMA 22

// Funciones Nucleo - UMC
#define RECIBIR_TAMANIO_PAGINA 0
#define MENSAJE_INICIALIZAR_PROGRAMA 1
#define MENSAJE_MATAR_PROGRAMA 2

typedef struct {
	int id_programa;
	int paginas_requeridas;
	char *codigo;
} t_programa_completo;

typedef struct {
	int tamanio;
} t_pagina_tamanio;

typedef struct {
	int pid;
} t_finalizar;

t_buffer *serializar_programa_completo(t_programa_completo *programa);

void deserializar_programa_completo(void *buffer, t_programa_completo *programa);

t_buffer *serializar_finalizar(t_finalizar *finalizar);

void deserializar_finalizar(void *buffer, t_finalizar *finalizar);

void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio);

#endif /* SERIALIZACION_NUCLEO_UMC_H_ */
