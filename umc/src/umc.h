/*
 * umc.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_

typedef struct {
	int puerto;
	char *ip_swap;
	int puerto_swap;
	int marcos;
	int marco_size;
	int marco_x_proc;
	int entradas_tlb;
	int retardo;
} t_config_umc;

void carga_configuracion_UMC(char *archivo, t_config_umc *configuracion);

void inicializar_programa();

void solicitar_bytes_pagina();

void almacenar_bytes_pagina();

void finalizar_programa();

void cambiar_retardo();

void generar_dump();

void limpiar_contenido();

#endif /* SRC_UMC_H_ */
