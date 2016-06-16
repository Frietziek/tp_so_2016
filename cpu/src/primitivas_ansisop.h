/*
 * primitivas_ansisop.h
 *
 *  Created on: 22/4/2016
 *      Author: utnso
 */

#ifndef SRC_PRIMITIVAS_ANSISOP_H_
#define SRC_PRIMITIVAS_ANSISOP_H_

#include <parser/parser.h>
#include <comunicaciones.h>
#include "comunicaciones_cpu.h"
#include "semaphore.h"
#include "serializaciones_cpu.h"
#include "semaforo_sockets_cpu.h"

// Funciones CPU - UMC
#define MENSAJE_LEER_PAGINA 1
#define MENSAJE_ESCRIBIR_PAGINA 2

// Funciones CPU - Nucleo
#define MENSAJE_LEER_COMPARTIDA 1
#define MENSAJE_ESCRIBIR_COMPARTIDA 2
#define MENSAJE_IMPRIMIR 3
#define MENSAJE_IMPRIMIR_TEXTO 4
#define MENSAJE_ENTRADA_SALIDA 5
#define MENSAJE_WAIT 6
#define MENSAJE_SIGNAL 7
#define MENSAJE_QUANTUM 8
#define MENSAJE_PROGRAMA_FINALIZADO 9
// Respuestas OK
#define RESPUESTA_LEER_COMPARTIDA 11
#define RESPUESTA_ESCRIBIR_COMPARTIDA 12
#define RESPUESTA_IMPRIMIR 13
#define RESPUESTA_IMPRIMIR_TEXTO 14
#define RESPUESTA_ENTRADA_SALIDA 15
// Respuesta de 16, me bloqueo
#define RESPUESTA_WAIT 16
// Cuando recibo un 16, respondo con un 56 y mando PCB
#define RESPUESTA_PCB 56
// Recibio el PCB correctamente
#define RESPUESTA_PCB_OK 66
#define ERROR_RESPUESTA_PCB 76
// Sigo con mi rafaga actual
#define RESPUESTA_SEGUI_RAFAGA 86
#define RESPUESTA_SIGNAL 17
#define RESPUESTA_QUANTUM 18
#define RESPUESTA_PROGRAMA_FINALIZADO 19
// Respuestas Error
#define ERROR_LEER_COMPARTIDA 21
#define ERROR_ESCRIBIR_COMPARTIDA 22
#define ERROR_IMPRIMIR 23
#define ERROR_IMPRIMIR_TEXTO 24
#define ERROR_ENTRADA_SALIDA 25
#define ERROR_WAIT 26
#define ERROR_SIGNAL 27
#define ERROR_QUANTUM 28
#define ERROR_PROGRAMA_FINALIZADO 29

// Funciones Nucleo - CPU
#define MENSAJE_PCB 1
#define MENSAJE_MATAR 2
// Respuestas OK
#define RESPUESTA_MATAR 12
// Respuestas Error
#define ERROR_PCB 21
#define ERROR_MATAR 22

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
