/*
 ============================================================================
 Name        : umc.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <comunicaciones.h>
#include "umc.h"

#include "deserializacion_cpu_umc.h"

int socket_swap;

int main(void) {
	int comando; // Comandos ingresados de la consola de UMC
	t_config_umc *configuracion = malloc(sizeof(t_config_umc)); // Estructura de configuracion de la UMC
	carga_configuracion_UMC("src/config.umc.ini", configuracion);

	// Se crea el bloque de la memoria principal
	int memoria_principal = calloc(configuracion->marcos,
			configuracion->marco_size);

	// TODO Crear estructuras para programas
	// TODO Crear Cache TLB

	// Crear servidor (socket host)
	t_configuracion_servidor *servidor_umc = malloc(
			sizeof(t_configuracion_servidor));
	servidor_umc->puerto = configuracion->puerto;
	crear_servidor(servidor_umc);
	printf("Servidor UMC corriendo\n");

	//Se realiza una conexión con el swap (server)
	socket_swap = conectar_servidor(configuracion->ip_swap,
			configuracion->puerto_swap);
	printf("UMC conectado con SWAP\n");
	enviar_mensaje(socket_swap, "Hola soy el UMC");

	// TODO Administrar los pedidos de las CPUs y el nucleo

	printf("Proceso UMC creado.\n");
	printf("Ingrese uno de los siguientes comandos para continuar:\n");
	printf("1 - Cambiar retardo de la consola UMC\n");
	printf("2 - Generar reporte y archivo Dump\n");
	printf("3 - Limpiar contenido de LTB o paginas\n");
	scanf("%d", &comando);
	// TODO Implementar funciones de la consola de UMC
	switch (comando) {
	case 1:
		printf("Entro en Retardo\n");
		cambiar_retardo();
		break;
	case 2:
		printf("Entro en Dump\n");
		generar_dump();
		break;
	case 3:
		printf("Entro en Flush\n");
		limpiar_contenido();
		break;
	default:
		printf("Comando no reconocido\n");
		break;
	}
	free(configuracion);
	return EXIT_SUCCESS;
}

void carga_configuracion_UMC(char *archivo, t_config_umc *configuracionUMC) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "PUERTO")) {
		configuracionUMC->puerto = config_get_int_value(configuracion,
				"PUERTO");
	}
	if (config_has_property(configuracion, "IP_SWAP")) {
		configuracionUMC->ip_swap = config_get_string_value(configuracion,
				"IP_SWAP");
	}
	if (config_has_property(configuracion, "PUERTO_SWAP")) {
		configuracionUMC->puerto_swap = config_get_int_value(configuracion,
				"PUERTO_SWAP");
	}
	if (config_has_property(configuracion, "MARCOS")) {
		configuracionUMC->marcos = config_get_int_value(configuracion,
				"MARCOS");
	}
	if (config_has_property(configuracion, "MARCO_SIZE")) {
		configuracionUMC->marco_size = config_get_int_value(configuracion,
				"MARCO_SIZE");
	}
	if (config_has_property(configuracion, "MARCO_X_PROC")) {
		configuracionUMC->marco_x_proc = config_get_int_value(configuracion,
				"MARCO_X_PROC");
	}
	if (config_has_property(configuracion, "ENTRADAS_TLB")) {
		configuracionUMC->entradas_tlb = config_get_int_value(configuracion,
				"ENTRADAS_TLB");
	}
	if (config_has_property(configuracion, "RETARDO")) {
		configuracionUMC->retardo = config_get_int_value(configuracion,
				"RETARDO");
	}
	free(configuracion);
}

/*void definir_variable(void *buffer) {
 t_variable_simple *variable = malloc(sizeof(t_variable_simple));

 deserializar_variable(buffer, variable);

 // TODO Quien se encarga de guardar el espacio de la variable?
 // Como se en que pagina debo escribir?
 }*/

void inicializar_programa(void *buffer) {
	enviar_mensaje(socket_swap, buffer);
	// TODO Responder al nucleo
}

void solicitar_bytes_pagina(void *buffer) {
	enviar_mensaje(socket_swap, buffer);
	// TODO Responder el pedido a la CPU
}

void almacenar_bytes_pagina(void *buffer) {
	enviar_mensaje(socket_swap, buffer);
	// TODO Responder el pedido a la CPU
}

void finalizar_programa(void *buffer) {
	enviar_mensaje(socket_swap, buffer);
	// TODO Recibir respuesta de Swap?
}

void cambiar_retardo() {
	// TODO Terminar funcion
}

void generar_dump() {
	// TODO Terminar funcion
}

void limpiar_contenido() {
	// TODO Terminar funcion
}

