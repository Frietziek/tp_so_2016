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

// Funciones CPU - UMC
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2

// Funciones CPU - Nucleo
#define MENSAJE_OBTENER_VALOR_COMPARTIDA 1
#define MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA 2
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4
#define MENSAJE_ENTRADA_SALIDA 5
#define MENSAJE_WAIT 6
#define MENSAJE_SIGNAL 7
#define MENSAJE_QUANTUM 8
#define MENSAJE_PROGRAMA_FINALIZADO 9
// Respuestas OK
#define RESPUESTA_OBTENER_VALOR_COMPARTIDA 11
#define RESPUESTA_ASIGNAR_VARIABLE_COMPARTIDA 12
#define RESPUESTA_IMPRIMIR 13
#define RESPUESTA_IMPRIMIR_TEXTO 14
#define RESPUESTA_ENTRADA_SALIDA 15
#define RESPUESTA_WAIT 16
#define RESPUESTA_SIGNAL 17
#define RESPUESTA_QUANTUM 18
#define RESPUESTA_PROGRAMA_FINALIZADO 19
// Respuestas Error
#define ERROR_OBTENER_VALOR_COMPARTIDA 21
#define ERROR_ASIGNAR_VARIABLE_COMPARTIDA 22
#define ERROR_IMPRIMIR 23
#define ERROR_IMPRIMIR_TEXTO 24
#define ERROR_ENTRADA_SALIDA 25
#define ERROR_WAIT 26
#define ERROR_SIGNAL 27
#define ERROR_QUANTUM 28
#define ERROR_PROGRAMA_FINALIZADO 29

t_puntero ansisop_definir_variable(t_nombre_variable identificador_variable);
t_puntero ansisop_obtener_posicion_variable(
		t_nombre_variable indentificador_variable);
t_valor_variable ansisop_derefenciar(t_puntero direccion_variable);
void ansisop_asignar(t_puntero direccion_variable, t_valor_variable valor);
t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable);
t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor);
void ansisop_ir_a_label(t_nombre_etiqueta etiqueta);
t_puntero_instruccion ansisop_llamar_funcion(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion);
void ansisop_retornar(t_valor_variable retorno);
void ansisop_imprimir(t_valor_variable valor_mostrar);
void ansisop_imprimir_texto(char* texto);
void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo);
void ansisop_wait(t_nombre_semaforo identificador_semaforo);
void ansisop_signal(t_nombre_semaforo identificador_semaforo);

#endif /* SRC_PRIMITIVAS_ANSISOP_H_ */
