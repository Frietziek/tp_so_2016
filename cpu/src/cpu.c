/*
 ============================================================================
 Name        : cpu.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <comunicaciones.h>
#include "primitivas_ansisop.h"
#include "cpu.h"
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
// Test para probar primitivas
static const char* DEFINICION_VARIABLES = "variables a, b, c";

int main(void) {
	int comando; // Comandos ingresados de la consola de CPU
	t_config_cpu *configuracion = malloc(sizeof(t_config_cpu));
	cargaConfiguracionCPU("config.cpu.ini", configuracion);
	printf("Proceso CPU creado.");

	int socket_nucleo = conectar_servidor("0.0.0.0", 5000);

	enviar_mensaje(socket_nucleo, "Hola soy el cpu");

	// TODO Recibir PCB del Nucleo
	// TODO Incrementar registro Program Counter en PCB
	// TODO Hacer el parser del indice de codigo

	// TODO Conectarse al UMC y recibir prox sentencia
	int socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc);
	printf("CPU conectado con UMC.\n");
	enviar_mensaje(socket_umc, "Hola soy el CPU");

	// TODO Ejecutar operaciones (Primitivas)
	// Test para probar primitivas
	printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
	analizadorLinea(strdup(DEFINICION_VARIABLES), &functions,
			&kernel_functions);

	// TODO Actualizar valores en UMC
	// TODO Actualizar PC en PCB
	// TODO Notificar al nucleo que termino el Quantum
	printf("Ingrese uno de los siguientes comandos para continuar:\n");
	printf("Cualquier tecla para cerrar el CPU\n");
	scanf("%d", &comando);
	switch (comando) {
	printf("Cerrando CPU.\n");
	break;
	}

	close(socket_umc);
	return EXIT_SUCCESS;
}

void cargaConfiguracionCPU(char *archivo, t_config_cpu *configuracionCPU) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "IP_NUCLEO")) {
		configuracionCPU->ip_nucleo = config_get_string_value(configuracion,
				"IP_NUCLEO");
	} else {
		configuracionCPU->ip_nucleo = DEF_IP_NUCLEO;
	}
	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracionCPU->puerto_nucleo = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
	} else {
		configuracionCPU->puerto_nucleo = DEF_PUERTO_NUCLEO;
	}
	if (config_has_property(configuracion, "IP_UMC")) {
		configuracionCPU->ip_umc = config_get_string_value(configuracion,
				"IP_UMC");
	} else {
		configuracionCPU->ip_umc = DEF_IP_UMC;
	}
	if (config_has_property(configuracion, "PUERTO_UMC")) {
		configuracionCPU->puerto_umc = config_get_int_value(configuracion,
				"PUERTO_UMC");
	} else {
		configuracionCPU->puerto_umc = DEF_PUERTO_UMC;
	}
	free(configuracion);
}
;

