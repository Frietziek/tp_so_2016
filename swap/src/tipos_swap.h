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
#define MENSAJE_LEER_PAGINA_PARA_ESCRIBIR 6
#define MENSAJE_ESCRIBIR_PAGINA_NUEVA 7

// Respuestas OK
#define RESPUESTA_LEER_PAGINA 11
#define RESPUESTA_ESCRIBIR_PAGINA 12
#define RESPUESTA_INICIALIZAR_PROGRAMA 13
#define RESPUESTA_FINALIZAR_PROGRAMA 14
#define RESPUESTA_LEER_PAGINA_PARA_ESCRIBIR 16
#define RESPUESTA_ESCRIBIR_PAGINA_NUEVA 17
// Respuestas Error
#define ERROR_LEER_PAGINA 21
#define ERROR_ESCRIBIR_PAGINA 22
#define ERROR_INICIALIZAR_PROGRAMA 23
#define ERROR_FINALIZAR_PROGRAMA 24
#define ERROR_LEER_PAGINA_PARA_ESCRIBIR 26
#define ERROR_ESCRIBIR_PAGINA_NUEVA 27

typedef struct {
	int puerto_escucha;
	char *nombre_swap;
	int cantidad_paginas;
	int tamano_pagina;
	int retardo_compactacion;
	int retardo_acceso;
} t_config_swap;

typedef struct {
	int id_programa;
	int cantidad_paginas;
	int pagina_inicial_swap;
} t_program_info;

#endif /* TIPOS_SWAP_H_ */
