/*
 * funciones_comunes.c
 *
 *  Created on: 16/6/2016
 *      Author: utnso
 */

#include "funciones_comunes.h"

int calcula_pagina(t_puntero instruccion) {
	int modulo = (int) (instruccion) % tamanio_pagina;
	int division = (int) (instruccion) / tamanio_pagina;
	return (modulo == 0) ? ++division : division;
}

int calcula_offset(t_puntero instruccion) {
	return (int) (instruccion) % tamanio_pagina;
}
