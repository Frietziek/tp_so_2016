/*
 * tipos_swap.h
 *
 *  Created on: Apr 22, 2016
 *      Author: utnso
 */

#ifndef TIPOS_SWAP_H_
#define TIPOS_SWAP_H_

// Handshake
#define MENSAJE_HANDSHAKE 0
#define RESPUESTA_HANDSHAKE 10
#define ERROR_HANDSHAKE 20

// Perticiones del UMC
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2
#define MENSAJE_INICIAR_PROGRAMA 3
#define MENSAJE_FINALIZAR_PROGRAMA 4

// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
#define RESPUESTA_INICIALIZAR_PROGRAMA 13
#define RESPUESTA_FINALIZAR_PROGRAMA 14
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22
#define ERROR_INICIALIZAR_PROGRAMA 23
#define ERROR_FINALIZAR_PROGRAMA 24


typedef struct {
	int puerto_escucha;
	char *nombre_swap;
	int cantidad_paginas;
	int tamano_pagina;
	int retardo_compactacion;
} t_config_swap;

typedef struct {
	int id_programa;
	int cantidad_paginas;
	int pagina_inicial_swap;
} t_program_info;

#endif /* TIPOS_SWAP_H_ */
