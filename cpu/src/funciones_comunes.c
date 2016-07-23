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

int calcula_offset_instruccion(t_puntero_instruccion start, int pagina) {
	return (pagina == 0) ? calcula_offset(start) : 0;
}

int instruccion_en_una_pagina(t_intructions *instruccion,
		t_pagina_pedido *pagina) {
	return (calcula_pagina(instruccion->start + instruccion->offset)
			== pagina->pagina);
}
