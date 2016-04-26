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

int main(void) {
	int comando; // Comandos ingresados de la consola de UMC
	t_config_umc *configuracion = malloc(sizeof(t_config_umc)); // Estructura de configuracion de la UMC
	cargaConfiguracionUMC("config.umc.ini", configuracion);

	// TODO Crear bloque de memoria principal
	// TODO Crear estructuras para programas
	// TODO Crear Cache TLB

	// Crear servidor (socket host)
	t_configuracion_servidor *servidor_umc = malloc(sizeof(t_configuracion_servidor));
	servidor_umc->puerto = configuracion->puerto;
	crear_servidor(servidor_umc);
	printf("Servidor UMC corriendo\n");

	//Se realiza una conexión con el swap (server)
	int socket_swap = conectar_servidor(configuracion->ip_swap, configuracion->puerto_swap);
	printf("UMC conectado con SWAP\n");
	enviar_mensaje(socket_swap, "Hola soy el UMC");

	// TODO Recibir conexiones de Nucleo y CPUs
	// TODO Crear hilos por cada conexion

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
			break;
		case 2:
			printf("Entro en Dump\n");
			break;
		case 3:
			printf("Entro en Flush\n");
			break;
		default:
			printf("Comando no reconocido\n");
			break;
	}
	free(configuracion);
	return EXIT_SUCCESS;
}

void cargaConfiguracionUMC(char *archivo, t_config_umc *configuracionUMC) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "PUERTO")) {
		configuracionUMC->puerto = config_get_int_value(configuracion, "PUERTO");
	} else {
		configuracionUMC->puerto = DEF_PUERTO;
	}
	if (config_has_property(configuracion, "IP_SWAP")) {
		configuracionUMC->ip_swap = config_get_string_value(configuracion, "IP_SWAP");
	} else {
		configuracionUMC->ip_swap = DEF_IP_SWAP;
	}
	if (config_has_property(configuracion, "PUERTO_SWAP")) {
		configuracionUMC->puerto_swap = config_get_int_value(configuracion, "PUERTO_SWAP");
	} else {
		configuracionUMC->puerto_swap = DEF_PUERTO_SWAP;
	}
	if (config_has_property(configuracion, "MARCOS")) {
		configuracionUMC->marcos = config_get_int_value(configuracion, "MARCOS");
	} else {
		configuracionUMC->marcos = DEF_MARCOS;
	}
	if (config_has_property(configuracion, "MARCO_SIZE")) {
		configuracionUMC->marco_size = config_get_int_value(configuracion, "MARCO_SIZE");
	} else {
		configuracionUMC->marco_size = DEF_MARCO_SIZE;
	}
	if (config_has_property(configuracion, "MARCO_X_PROC")) {
		configuracionUMC->marco_x_proc = config_get_int_value(configuracion, "MARCO_X_PROC");
	} else {
		configuracionUMC->marco_x_proc = DEF_MARCO_X_PROC;
	}
	if (config_has_property(configuracion, "ENTRADAS_TLB")) {
		configuracionUMC->entradas_tlb = config_get_int_value(configuracion, "ENTRADAS_TLB");
	} else {
		configuracionUMC->entradas_tlb = DEF_ENTRADAS_TLB;
	}
	if (config_has_property(configuracion, "RETARDO")) {
		configuracionUMC->retardo = config_get_int_value(configuracion, "RETARDO");
	} else {
		configuracionUMC->retardo = DEF_RETARDO;
	}
	free(configuracion);
}
