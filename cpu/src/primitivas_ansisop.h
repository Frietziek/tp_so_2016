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
#include "funciones_comunes.h"
#include "mensajes_cpu.h"

t_puntero ansisop_definir_variable(t_nombre_variable identificador_variable);
t_puntero ansisop_obtener_posicion_variable(
		t_nombre_variable indentificador_variable);
t_valor_variable ansisop_derefenciar(t_puntero direccion_variable);
void ansisop_asignar(t_puntero direccion_variable, t_valor_variable valor);
t_valor_variable ansisop_obtener_valor_compartida(t_nombre_compartida variable);
t_valor_variable ansisop_asignar_valor_compartida(t_nombre_compartida variable,
		t_valor_variable valor);
void ansisop_ir_a_label(t_nombre_etiqueta etiqueta);
void ansisop_llamar_con_retorno(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar);
void ansisop_retornar(t_valor_variable retorno);
void ansisop_imprimir(t_valor_variable valor_mostrar);
void ansisop_imprimir_texto(char* texto);
void ansisop_entrada_salida(t_nombre_dispositivo dispositivo, int tiempo);
void ansisop_wait(t_nombre_semaforo identificador_semaforo);
void ansisop_signal(t_nombre_semaforo identificador_semaforo);
void ansisop_finalizar();
// Funciones usadas por primitivas
t_indice_stack* posiciono_indice_stack();
t_variables_stack* posiciono_indice_variables(t_indice_stack* indice_stack);
void enviar_entrada_salida(t_buffer * buffer);
int calcula_paginas_variable(t_puntero direccion_variable);
void leo_variable_desde_UMC(t_puntero direccion_variable, int pagina);
int calcula_tamanio_variable(t_puntero direccion_variable,
		t_pagina_pedido *p_pagina, int pagina);
int variable_en_una_pagina(t_puntero direccion_variable,
		t_pagina_pedido *pagina);
#endif /* SRC_PRIMITIVAS_ANSISOP_H_ */
