/*
 * utilidades_swap.h
 *
 *  Created on: Apr 22, 2016
 *      Author: utnso
 */

#ifndef UTILIDADES_SWAP_H_
#define UTILIDADES_SWAP_H_

void cargar_configuracion_swap(char *archivo, t_config_swap *configuracion);
int crear_archivo_swap(char *nombre_archivo, int tamano_archivo);
void inicializar_bitmap(t_bitarray *bitmap);
int encontrar_ubicacion_libre(int paginas_requeridas, t_bitarray *paginas_bitmap);
int hay_espacio_total_disponible(int cantidad_paginas_requeridas, t_bitarray *paginas_bitmap);
void reservar_lugar_para_el_programa(int numero_pagina_inicial, int paginas_requeridas, t_bitarray *paginas_bitmap);
void liberar_lugar_del_programa(int numero_pagina_inicial, int paginas_ocupadas, t_bitarray *paginas_bitmap);
t_program_info *buscar_programa(int id_programa, t_list *lista_programas);
t_program_info *buscar_programa_por_pagina_inicial(int numero_pagina_inicial, t_list *lista_programas);
void eliminar_programa(int id_programa, t_list *lista_programas);
void simular_espera(int milisegundos);
int reescribir_programa(t_program_info * el_programa, int corrimiento_hacia_izquierda, FILE *archivo_swap, t_bitarray *paginas_bitmap, t_config_swap *config_swap, t_log *loggerManager);
#endif /* UTILIDADES_SWAP_H_ */
