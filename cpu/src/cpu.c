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
#include <comunicaciones.h>

#include "primitivas_ansisop.h"

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
//static const char* DEFINICION_VARIABLES = "variables a, b, c";

int main(void) {
	int comando; // Comandos ingresados de la consola de CPU

	// TODO Conectar con el Nucleo (socket client)
	// TODO Recibir PCB del Nucleo
	// TODO Incrementar registro Program Counter en PCB
	// TODO Hacer el parser del indice de codigo

	// TODO Conectarse al UMC y recibir prox sentencia
	int socket_umc = conectar_servidor("0.0.0.0", 3603);
	printf("CPU conectado con UMC.\n");
	enviar_mensaje(socket_umc, "Hola soy el CPU");

	// TODO Ejecutar operaciones (Primitivas)
	// Test para probar primitivas
	//printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
	//analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);

	// TODO Actualizar valores en UMC
	// TODO Actualizar PC en PCB
	// TODO Notificar al nucleo que termino el Quantum
	puts("Proceso CPU creado.");
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
