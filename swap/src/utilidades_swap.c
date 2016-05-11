#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
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


