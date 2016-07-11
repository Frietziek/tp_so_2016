#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "tipos_swap.h"
#include "utilidades_swap.h"
#include "serializacion_swap_umc.h"

/*------------------------- Valores por defecto de la configuracion del SWAP -------------------------*/
#define DEFAULT_PUERTO_ESCUCHA 6000
#define DEFAULT_NOMBRE_SWAP "swap.data"
#define DEFAULT_CANTIDAD_PAGINAS 512
#define DEFAULT_TAMANO_PAGINA 256
#define DEFAULT_RETARDO_COMPACTACION 60000
#define DEFAULT_RETARDO_ACCESO 600
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

	if (config_has_property(archivo_configuracion, "RETARDO_ACCESO")) {
			configuracion_swap->retardo_acceso = config_get_int_value(
			archivo_configuracion, "RETARDO_ACCESO");
	} else {
		configuracion_swap->retardo_acceso = DEFAULT_RETARDO_ACCESO;
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

//Devuelve 1 si hay espacio, 0 en caso de que no
int hay_espacio_total_disponible(int cantidad_paginas_requeridas, t_bitarray *paginas_bitmap){
	int index;
	int cantidad_frames_totales = bitarray_get_max_bit(paginas_bitmap);
	int cantidad_frames_libres = 0;

	for(index = 0 ; index < cantidad_frames_totales ; index ++){
		if (!bitarray_test_bit(paginas_bitmap, index))
			cantidad_frames_libres ++;
	}

	return (cantidad_frames_libres >= cantidad_paginas_requeridas);
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

//Busca y retorna la estructura de control del programa cuyo número de pagina inicial es el solicitado
t_program_info *buscar_programa_por_pagina_inicial(int numero_pagina_inicial, t_list *lista_programas){

	bool busqueda_programa_logica(t_program_info *program_info){
		return (numero_pagina_inicial == program_info->pagina_inicial_swap);
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

//Realiza una system call que hace una espera en la ejecución de x milisegundos
void simular_espera(int milisegundos){
	usleep(milisegundos * 1000); //Multiplico por 1000 para obtener el equivalente en microsegundos
}

//Lee un programa entero (todas sus páginas), retorna 0 en caso de exito, y -1 en caso de error
int leer_programa_entero(t_program_info * el_programa, FILE *archivo_swap, void *buffer, t_config_swap *config_swap, t_log *loggerManager){

		int posicion_lectura = el_programa->pagina_inicial_swap * config_swap->tamano_pagina;
		int tamano_lectura = el_programa->cantidad_paginas * config_swap->tamano_pagina;

		fseek(archivo_swap, posicion_lectura, SEEK_SET);

		int cantidad_leida = fread(buffer,tamano_lectura, 1, archivo_swap);

		if (cantidad_leida == 1){//Bloques leidos
			log_trace(loggerManager,"[Compactacion] Se leyeron correctamente %i bloque de %i bytes", cantidad_leida, tamano_lectura);
			return 0;
		}
		else {
			log_error(loggerManager,"[Compactacion] Ocurrio un problema, se leyeron %i bloques de %i bytes", cantidad_leida, tamano_lectura);
			return -1;
		}
}

//Escribe un programa entero (todas sus páginas), retorna 0 en caso de exito, y -1 en caso de error
int escribir_programa_entero(t_program_info * el_programa, FILE *archivo_swap, void *buffer_programa, t_config_swap *config_swap, t_log *loggerManager){

	int posicion_escritura = el_programa->pagina_inicial_swap * config_swap->tamano_pagina ;
	int tamano_escritura = el_programa->cantidad_paginas * config_swap->tamano_pagina;

	fseek(archivo_swap, posicion_escritura, SEEK_SET);

	int cantidad_escrita = fwrite(buffer_programa ,1, tamano_escritura ,archivo_swap);

	if (cantidad_escrita == tamano_escritura){
		log_trace(loggerManager,"[Compactación] Se escribieron correctamente %i bytes", cantidad_escrita);
		return 0;
	}
	else {
		log_error(loggerManager,"[Compactación] Ocurrio un problema, se escribieron %i de %i bytes", cantidad_escrita, tamano_escritura);
		return -1;
	}
}

//Reescribe un programa con un corrimiento hacia la izquierda, afectando al archivo swap y a todas las estructuras de control asociadas. Retorna -1 en caso de error, 0 en caso de exito
int reescribir_programa(t_program_info * el_programa, int corrimiento_hacia_izquierda, FILE *archivo_swap, t_bitarray *paginas_bitmap, t_config_swap *config_swap, t_log *loggerManager){

	//Lectura del programa en un buffer auxiliar
	void *buffer_programa = malloc(el_programa->cantidad_paginas * config_swap->tamano_pagina); //TODO: Prestar atención a que esta linea esté bien al testear
	int status_lectura = leer_programa_entero(el_programa, archivo_swap, buffer_programa, config_swap, loggerManager);

	if(status_lectura != 0) //Error
		return -1;

	//Se libera el espacio reservado en el bitmap
	liberar_lugar_del_programa(el_programa->pagina_inicial_swap, el_programa->cantidad_paginas, paginas_bitmap);

	//Se actualiza el_programa con la nueva posición inicial TODO: Verificar que tenga impacto en la lista per se, debería porque es un puntero
	el_programa->pagina_inicial_swap -= corrimiento_hacia_izquierda;


	//Se escribe el programa en la nueva ubicacion
	int status_escritura = escribir_programa_entero(el_programa, archivo_swap, buffer_programa, config_swap, loggerManager);

	if(status_escritura != 0) //Error
		return -1;

	//Se reserva el nuevo espacio en el bitmap
	reservar_lugar_para_el_programa(el_programa->pagina_inicial_swap, el_programa->cantidad_paginas, paginas_bitmap);

	return 0;
}
