/*
 * primitivas_ansisop.h
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#ifndef SRC_PRIMITIVAS_ANSISOP_H_
#define SRC_PRIMITIVAS_ANSISOP_H_

#include <stdio.h>
#include <parser/parser.h>

t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable(t_nombre_variable indentificador_variable);
t_valor_variable derefenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable, t_valor_variable valor);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
void irAlLabel(t_nombre_etiqueta etiqueta);
t_puntero_instruccion llamarFuncion(t_nombre_etiqueta etiqueta, t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion);
void retornar(t_valor_variable retorno);
void imprimir(t_valor_variable valor_mostrar);
void imprimirTexto(char* texto);
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);
void wait(t_nombre_semaforo identificador_semaforo);
void signal(t_nombre_semaforo identificador_semaforo);

#endif /* SRC_PRIMITIVAS_ANSISOP_H_ */
