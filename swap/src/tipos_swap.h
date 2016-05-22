/*
 * tipos_swap.h
 *
 *  Created on: Apr 22, 2016
 *      Author: utnso
 */

#ifndef TIPOS_SWAP_H_
#define TIPOS_SWAP_H_

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
