/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "semaphore.h"
//#include "cpu.h"
#include "primitivas_ansisop.h"
#include "semaforo_cpu.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero ansisop_definir_variable(t_nombre_variable identificador_variable) {
	printf("Se define la variable %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_puntero ansisop_obtener_posicion_variable(t_nombre_variable identificador_variable) {
	printf("Obtiene la posicion de %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_valor_variable ansisop_derefenciar(t_puntero direccion_variable) {
	// TODO Rellenar con los valores reales
	leer_pagina(5, 4, 4);
	printf("Espero semaforo\n");
	sem_wait(&s_pagina);
	printf("Signal recibido\n");
	printf("Dereferencia %d y su valor es %i\n", direccion_variable,
			(int) valor_pagina);
	return CONTENIDO_VARIABLE;
}

void ansisop_asignar(t_puntero direccion_variable, t_valor_variable valor) {
	// TODO Rellenar con los valores reales
	escribir_pagina(5, 4, 4,  4);
	printf("Asignando en %d el valor %d\n", direccion_variable, valor);
}

t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable) {
	obtener_valor_compartida(variable);
	printf("Obtiene valor de variable compartida %s\n", variable);
	return CONTENIDO_VARIABLE;
}

t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	asignar_valor_compartida(variable, valor);
	printf("Asignando en %s el valor %d\n", variable, valor);
	return CONTENIDO_VARIABLE;
}

void ansisop_ir_a_label(t_nombre_etiqueta etiqueta) {
	printf("Yendo al Label: %s\n", etiqueta);
}

t_puntero_instruccion ansisop_llamar_funcion(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion) {
	printf(
			"Llamando a la funcion de etiqueta %s, en el lugar de retorno %c y posicion de instruccion %c\n",
			etiqueta, donde_retornar, linea_en_ejecucion);
	return POSICION_MEMORIA;
}

void ansisop_retornar(t_valor_variable retorno) {
	printf("Retornando el valor de la variable %d\n", retorno);
}

void ansisop_imprimir(t_valor_variable valor_mostrar) {
	printf("Imprimiendo el valor: %d\n", valor_mostrar);
}

void ansisop_imprimir_texto(char* texto) {
	printf("Imprimiendo el texto: %s", texto);
}

void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo) {
	printf("El dispositivo %s, tiene un tiempo de %d", dispositivo, tiempo);
}

void ansisop_wait(t_nombre_semaforo identificador_semaforo) {
	printf("Esperando al semaforo %s", identificador_semaforo);
}

void ansisop_signal(t_nombre_semaforo identificador_semaforo) {
	printf("Semaforo activado %s", identificador_semaforo);
}
