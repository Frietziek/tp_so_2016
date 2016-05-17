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
#include <serializacion.h>
#include <comunicaciones.h>
#include "primitivas_ansisop.h"
#include "cpu.h"

// Test para probar primitivas
static const char* DEFINICION_VARIABLES = "variables a, b, c";

int main(void) {
	int comando; // Comandos ingresados de la consola de CPU
	t_config_cpu *configuracion = malloc(sizeof(t_config_cpu));
	carga_configuracion_cpu("src/config.cpu.ini", configuracion);
	printf("Proceso CPU creado.");

	int socket_nucleo = conectar_servidor(configuracion->ip_nucleo,
			configuracion->puerto_nucleo);

	t_persona *persona = malloc(sizeof(t_persona));
	persona->edad = 22;
	persona->cp = 1414;
	persona->nombre = "santi";
	persona->apellido = "bbb";

	t_buffer *buffer = serializar_persona(persona);

	enviar_buffer(socket_nucleo, buffer);

	free(persona);
	free(buffer);

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

	free(configuracion);
	close(socket_umc);
	return EXIT_SUCCESS;
}

void carga_configuracion_cpu(char *archivo, t_config_cpu *configuracion_cpu) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "IP_NUCLEO")) {
		configuracion_cpu->ip_nucleo = config_get_string_value(configuracion,
				"IP_NUCLEO");
	}
	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracion_cpu->puerto_nucleo = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
	}
	if (config_has_property(configuracion, "IP_UMC")) {
		configuracion_cpu->ip_umc = config_get_string_value(configuracion,
				"IP_UMC");
	}
	if (config_has_property(configuracion, "PUERTO_UMC")) {
		configuracion_cpu->puerto_umc = config_get_int_value(configuracion,
				"PUERTO_UMC");
	}
	free(configuracion);
}
