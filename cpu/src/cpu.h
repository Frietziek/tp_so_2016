/*
 * cpu.h
 *
 *  Created on: 26/4/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

typedef struct {
	char *ip_nucleo;
	int puerto_nucleo;
	char *ip_umc;
	int puerto_umc;
} t_config_cpu;

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion);

AnSISOP_funciones functions = {
		.AnSISOP_definirVariable = definirVariable,
		.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
		.AnSISOP_dereferenciar = derefenciar,
		.AnSISOP_asignar = asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel = irAlLabel,
		.AnSISOP_retornar = retornar,
		.AnSISOP_imprimir = imprimir,
		.AnSISOP_imprimirTexto = imprimirTexto,
		.AnSISOP_entradaSalida = entradaSalida
};

AnSISOP_kernel kernel_functions = {
		.AnSISOP_wait = wait,
		.AnSISOP_signal = signal
};

// Funciones CPU - UMC
void definir_variable(char *variable);
void obtener_posicion_variable(char * variable);
void dereferenciar(int pagina, int offset, int tamanio);
void asignar_variable(int pagina, int offset, int tamanio, int valor);

// Funciones CPU - Nucleo
void obtener_valor_compartida(char *variable);
void asignar_valor_compartida(char *variable, int valor);
void imprimir_variable(char *variable, int valor);
void imprimir_texto(char *texto);
void entrada_salida(char *nombre, int tiempo);
void wait_semaforo(char *semaforo);
void signal_semaforo(char *semaforo);


#endif /* CPU_H_ */
