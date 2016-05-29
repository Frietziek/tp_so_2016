/*
 * funciones_swap.h
 *
 *  Created on: May 29, 2016
 *      Author: utnso
 */

#ifndef FUNCIONES_SWAP_H_
#define FUNCIONES_SWAP_H_

int inicializar_programa(t_programa_completo *inicio_programa_info, t_list *lista_programas, t_bitarray *paginas_bitmap, t_log *loggerManager);
int finalizar_programa(t_programa *fin_programa_info, t_bitarray *paginas_bitmap, t_list *lista_programas, t_log *loggerManager);
int leer_bytes_swap(t_pagina *leer_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager, void *buffer);
int escribir_bytes_swap(t_pagina_completa *escribir_pagina_info, FILE *archivo_swap, t_config_swap *config_swap, t_log *loggerManager);
void atender_UMC(t_paquete *paquete, int socket_conexion);

#endif /* FUNCIONES_SWAP_H_ */
