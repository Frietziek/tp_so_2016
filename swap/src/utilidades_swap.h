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
void reservar_lugar_para_el_programa(int numero_pagina_inicial, int paginas_requeridas, t_bitarray *paginas_bitmap);
void liberar_lugar_del_programa(int numero_pagina_inicial, int paginas_ocupadas, t_bitarray *paginas_bitmap);
t_program_info *buscar_programa(int id_programa, t_list *lista_programas);
void eliminar_programa(int id_programa, t_list *lista_programas);
#endif /* UTILIDADES_SWAP_H_ */
