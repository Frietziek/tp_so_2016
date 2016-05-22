#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include "tipos_swap.h"
#include "utilidades_swap.h"

/*------------------------- Valores por defecto de la configuracion del SWAP -------------------------*/
#define DEFAULT_PUERTO_ESCUCHA 6000
#define DEFAULT_NOMBRE_SWAP "swap.data"
#define DEFAULT_CANTIDAD_PAGINAS 512
#define DEFAULT_TAMANO_PAGINA 256
#define DEFAULT_RETARDO_COMPACTACION 60000
/*----------------------------------------------------------------------------------------------------*/

void cargar_configuracion_swap(char *archivo, t_config_swap *configuracion_swap) {

	t_config *archivo_configuracion = malloc(sizeof(t_config));
	archivo_configuracion = config_create(archivo);

	if (config_has_property(archivo_configuracion, "PUERTO_ESCUCHA")) {
		configuracion_swap->puerto_escucha = config_get_int_value(
				archivo_configuracion, "PUERTO_ESCUCHA");
	} else {
		configuracion_swap->puerto_escucha = DEFAULT_PUERTO_ESCUCHA;
	}

	printf("%d", configuracion_swap->puerto_escucha);
	if (config_has_property(archivo_configuracion, "NOMBRE_SWAP")) {
		configuracion_swap->nombre_swap = config_get_string_value(
				archivo_configuracion, "NOMBRE_SWAP");
	} else {
		configuracion_swap->nombre_swap = DEFAULT_NOMBRE_SWAP;
	}

	if (config_has_property(archivo_configuracion, "CANTIDAD_PAGINAS")) {
		configuracion_swap->cantidad_paginas = config_get_int_value(
				archivo_configuracion, "CANTIDAD_PAGINAS");
	} else {
		configuracion_swap->cantidad_paginas = DEFAULT_CANTIDAD_PAGINAS;
	}

	if (config_has_property(archivo_configuracion, "TAMANO_PAGINA")) {
		configuracion_swap->tamano_pagina = config_get_int_value(
				archivo_configuracion, "TAMANO_PAGINA");
	} else {
		configuracion_swap->tamano_pagina = DEFAULT_TAMANO_PAGINA;
	}

	if (config_has_property(archivo_configuracion, "RETARDO_COMPACTACION")) {
		configuracion_swap->retardo_compactacion = config_get_int_value(
				archivo_configuracion, "RETARDO_COMPACTACION");
	} else {
		configuracion_swap->retardo_compactacion = DEFAULT_RETARDO_COMPACTACION;
	}

	free(archivo_configuracion);
}


int crear_archivo_swap(char *nombre_archivo, int tamano_archivo){
	char* comando = string_from_format("dd if=/dev/zero of=%s count=1 bs=%d", nombre_archivo, tamano_archivo);
	int status = system(comando);

	return status;
}


void inicializar_bitmap(t_bitarray *bitmap){
	int tamano_bitmap = bitarray_get_max_bit(bitmap);
	int index;
	for(index = 0 ; index < tamano_bitmap ; index++){
		bitarray_clean_bit(bitmap, index);
	}
}


/* Busca un bloque libre del tamano solicitado en el swap, y retorna el numero de pagina inicial o -1 en caso de no encontrar el suficiente espacio continuo     */
int encontrar_ubicacion_libre(int paginas_requeridas, t_bitarray *paginas_bitmap){
	int index;
	int cantidad_de_bits = bitarray_get_max_bit(paginas_bitmap);
	int numero_pagina_inicio = -1; //Por defecto no encontro un bloque continuo con la capacidad solicitada
	int cantidad_paginas_libres_encontradas = 0;

	for(index = 0 ; index < cantidad_de_bits ; index ++){

			if(bitarray_test_bit(paginas_bitmap, index)){
				cantidad_paginas_libres_encontradas = 0;
			}else{

				cantidad_paginas_libres_encontradas ++;

				if(cantidad_paginas_libres_encontradas >= paginas_requeridas){
					numero_pagina_inicio = index - cantidad_paginas_libres_encontradas + 1; //TODO: Comprobar que esta cuentita este dando bien
					break; //Salgo del ciclo porque encontre lo que queria
				}


			}
		}

	return numero_pagina_inicio;

}


//Setea los bits del bitmap en 1, para marcarlos como ocupados por el programa
void reservar_lugar_para_el_programa(int numero_pagina_inicial, int paginas_requeridas, t_bitarray *paginas_bitmap){
	int index;
	for(index = 0 ; index < paginas_requeridas; index ++){
		bitarray_set_bit(paginas_bitmap, index + numero_pagina_inicial);
	}
}

//Setea los bits del bitmap en 0, para marcarlos como libres
void liberar_lugar_del_programa(int numero_pagina_inicial, int paginas_ocupadas, t_bitarray *paginas_bitmap){
	int index;
	for(index = 0 ; index < paginas_ocupadas ; index ++){
		bitarray_clean_bit(paginas_bitmap, index + numero_pagina_inicial);
	}
}


//Busca y retorna la estructura de control del programa solicitado
t_program_info *buscar_programa(int id_programa, t_list *lista_programas){

	bool busqueda_programa_logica(t_program_info *program_info){
		return (id_programa == program_info->id_programa);
	}

	return list_find(lista_programas, (void *) busqueda_programa_logica);
}

//Elimina la estructura de control del programa
void eliminar_programa(int PID, t_list *lista_programas){

	bool eliminar_programa_logica(t_program_info *program_info){
			return (PID == program_info->id_programa);
	}

	void program_info_destroy(t_program_info *self) {
	    free(self);
	}

	list_remove_and_destroy_by_condition(lista_programas, (void *) eliminar_programa_logica, (void *) program_info_destroy);
}



