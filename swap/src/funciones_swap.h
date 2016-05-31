/*
 * funciones_swap.h
 *
 *  Created on: May 29, 2016
 *      Author: utnso
 */

#ifndef FUNCIONES_SWAP_H_
#define FUNCIONES_SWAP_H_

int inicializar_programa(t_programa_completo *inicio_programa_info);
int finalizar_programa(t_programa *fin_programa_info);
int leer_bytes_swap(t_pagina *leer_pagina_info, void *buffer);
int escribir_bytes_swap(t_pagina_completa *escribir_pagina_info);
void atender_UMC(t_paquete *paquete, int socket_conexion);

#endif /* FUNCIONES_SWAP_H_ */
