/*
 * primitivas_ansisop.c
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#include "primitivas_ansisop.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero definirVariable(t_nombre_variable identificador_variable) {
	printf("Se define la variable %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	printf("Obtiene la posicion de %c\n", identificador_variable);
	return POSICION_MEMORIA;
}

t_valor_variable derefenciar(t_puntero direccion_variable) {
	printf("Dereferencia %d y su valor es %d\n", direccion_variable, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	printf("Asignando en %d el valor %d\n", direccion_variable, valor);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {
	printf("Obtiene valor de variable compartida %s\n", variable);
	return CONTENIDO_VARIABLE;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {
	printf("Asignando en %s el valor %d\n", variable, valor);
	return CONTENIDO_VARIABLE;
}

void irAlLabel(t_nombre_etiqueta etiqueta) {
	printf("Yendo al Label: %s\n", etiqueta);
}

t_puntero_instruccion llamarFuncion(t_nombre_etiqueta etiqueta, t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion) {
	printf("Llamando a la funcion de etiqueta %s, en el lugar de retorno %c y posicion de instruccion %c\n", etiqueta, donde_retornar, linea_en_ejecucion);
	return POSICION_MEMORIA;
}

void retornar(t_valor_variable retorno) {
	printf("Retornando el valor de la variable %d\n", retorno);
}

void imprimir(t_valor_variable valor_mostrar) {
	printf("Imprimiendo el valor: %d\n", valor_mostrar);
}

void imprimirTexto(char* texto) {
	printf("Imprimiendo el texto: %s", texto);
}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {
	printf("El dispositivo %s, tiene un tiempo de %d", dispositivo, tiempo);
}

void wait(t_nombre_semaforo identificador_semaforo) {
	printf("Esperando al semaforo %s", identificador_semaforo);
}

void signal(t_nombre_semaforo identificador_semaforo) {
	printf("Semaforo activado %s", identificador_semaforo);
}
