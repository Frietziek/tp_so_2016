/*
 ============================================================================
 Name        : swap.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "tipos_swap.h"

int main(void) {
	t_config_swap *config_swap = malloc(sizeof(t_config_swap));
	cargar_configuracion_swap("config.swap.ini", config_swap);

	int a = 2;

	return EXIT_SUCCESS;
}
