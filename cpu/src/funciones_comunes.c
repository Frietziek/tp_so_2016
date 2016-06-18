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
	if (modulo == 0) {
		++division;
	}
	return division;
}

int calcula_offset(t_puntero instruccion) {
	return (int) (instruccion) % tamanio_pagina;
}
