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

#include "serializacion_cpu_nucleo.h"
#include "serializacion_cpu_umc.h"

// Test para probar primitivas
static const char* DEFINICION_VARIABLES = "variables a, b, c";

// Sockets de los procesos a los cuales me conecto
int socket_nucleo;
int socket_umc;

int main(void) {
	int comando; // Comandos ingresados de la consola de CPU
	t_config_cpu *configuracion = malloc(sizeof(t_config_cpu));
	carga_configuracion_cpu("src/config.cpu.ini", configuracion);
	printf("Proceso CPU creado.\n");

	if ((socket_nucleo = conectar_servidor(configuracion->ip_nucleo,
			configuracion->puerto_nucleo)) > 0) {
		printf("CPU conectado con Nucleo\n");
		handshake_cpu_nucleo();
	} else {
		perror("Error al conectarse con el Nucleo");
	}

	// INICIO EJEMPLO ENVIO PAQUETE SIN PAYLOAD

	/*t_header *header_handshake = malloc(sizeof(t_header));
	 header_handshake->id_proceso_emisor = PROCESO_CPU;
	 header_handshake->id_proceso_receptor = PROCESO_NUCLEO;
	 header_handshake->id_mensaje = MENSAJE_HANDSHAKE;
	 header_handshake->longitud_mensaje = 0;
	 enviar_header(socket_nucleo, header_handshake);
	 free(header_handshake);*/

	// FIN EJEMPLO ENVIO PAQUETE SIN PAYLOAD
	// INICIO EJEMPLO ENVIO PAQUETE CON PAYLOAD
	t_persona *persona = malloc(sizeof(t_persona));
	persona->nombre = malloc(9);
	persona->apellido = malloc(5);
	persona->materias_aprobadas = 44;
	persona->edad = 2223;
	persona->cp = 14141;
	persona->nombre = "santiago";
	persona->apellido = "bose";

	t_buffer *buffer_persona = serializar_persona(persona);
	t_header *header = malloc(sizeof(t_header));

	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = 1;
	header->longitud_mensaje = buffer_persona->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, buffer_persona)
			< sizeof(t_header) + buffer_persona->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(persona);
	free(buffer_persona);
	free(header);

	// FIN EJEMPLO ENVIO PAQUETE CON PAYLOAD

	// TODO Recibir PCB del Nucleo
	// TODO Incrementar registro Program Counter en PCB
	// TODO Hacer el parser del indice de codigo

	// TODO Conectarse al UMC y recibir prox sentencia
	if ((socket_umc = conectar_servidor(configuracion->ip_umc,
			configuracion->puerto_umc)) > 0) {
		printf("CPU conectado con UMC.\n");
		handshake_cpu_umc();
	} else {
		perror("Error al conectarse con la UMC\n");
	}

	// Test para probar la comunicacion de funciones con el UMC
	dereferenciar(5, 10, 4);

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

// Funciones CPU - UMC
/*void definir_variable(char *variable) {
 t_variable *p_variable = malloc(sizeof(t_variable));
 p_variable->nombre = variable;
 //t_buffer *p_buffer = serializar_variable(p_variable);
 // t_buffer *h_buffer; // TODO hacer el header
 t_buffer *buffer; // TODO Rellenar con header y payload
 enviar_mensaje(socket_umc, buffer);
 free(p_variable);
 }*/

/*void obtener_posicion_variable(char * variable) {
 t_variable *p_variable = malloc(sizeof(t_variable));
 p_variable->nombre = variable;
 //t_buffer *p_buffer = serializar_variable(p_variable);
 // t_buffer *h_buffer; // TODO hacer el header
 t_buffer *buffer; // TODO Rellenar con header y payload
 enviar_mensaje(socket_umc, buffer);
 free(p_variable);
 }*/

void handshake_cpu_umc() {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_UMC;
	header->id_mensaje = MENSAJE_HANDSHAKE;
	header->longitud_mensaje = 0;

	enviar_header(socket_umc, header);
	free(header);
}

void dereferenciar(int pagina, int offset, int tamanio) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_UMC;
	header->id_mensaje = MENSAJE_DEREFENCIAR;

	t_pagina *p_derefenciar = malloc(sizeof(t_pagina));
	p_derefenciar->pagina = pagina;
	p_derefenciar->offset = offset;
	p_derefenciar->tamanio = tamanio;
	t_buffer *p_buffer = serializar_dereferenciar(p_derefenciar);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_derefenciar);
	free(p_buffer);
}

void asignar_variable(int pagina, int offset, int tamanio, int valor) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_UMC;
	header->id_mensaje = MENSAJE_ASIGNAR_VARIABLE;

	t_pagina_completa *p_asignar = malloc(sizeof(t_pagina_completa));
	p_asignar->pagina = pagina;
	p_asignar->offset = offset;
	p_asignar->tamanio = tamanio;
	p_asignar->valor = valor;

	t_buffer * p_buffer = serializar_asignar(p_asignar);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_asignar);
	free(p_buffer);
}

// Funciones CPU - Nucleo
void handshake_cpu_nucleo() {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_HANDSHAKE;
	header->longitud_mensaje = 0;

	enviar_header(socket_nucleo, header);
	free(header);
}

void obtener_valor_compartida(char *variable) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_OBTENER_VALOR_COMPARTIDA;

	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;

	t_buffer * p_buffer = serializar_variable_compartida(p_compartida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);
}

void asignar_valor_compartida(char *variable, int valor) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA;

	t_variable_completa *p_compartida = malloc(sizeof(t_variable_completa));
	p_compartida->nombre = variable;
	p_compartida->valor = valor;

	t_buffer * p_buffer = serializar_asignar_variable_compartida(p_compartida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_compartida);
	free(p_buffer);
}

void imprimir_variable(char *variable, int valor) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_IMPRIMIR;

	t_variable_completa *p_variable = malloc(sizeof(t_variable_completa));
	p_variable->nombre = variable;
	p_variable->valor = valor;

	t_buffer * p_buffer = serializar_imprimir_variable(p_variable);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_variable);
	free(p_buffer);
}

void imprimir_texto(char *texto) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_IMPRIMIR_TEXTO;

	t_texto *p_texto = malloc(sizeof(t_texto));
	p_texto->texto = texto;

	t_buffer * p_buffer = serializar_imprimir_texto(p_texto);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_texto);
	free(p_buffer);
}

void entrada_salida(char *nombre, int tiempo) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_ENTRADA_SALIDA;

	t_entrada_salida *p_entrada_salida = malloc(sizeof(t_entrada_salida));
	p_entrada_salida->nombre_dispositivo = nombre;
	p_entrada_salida->tiempo = tiempo;

	t_buffer * p_buffer = serializar_entrada_salida(p_entrada_salida);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_entrada_salida);
	free(p_buffer);
}

void wait_semaforo(char *semaforo) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_WAIT;

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;

	t_buffer * p_buffer = serializar_semaforo(p_semaforo);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}

void signal_semaforo(char *semaforo) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CPU;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_SIGNAL;

	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;

	t_buffer * p_buffer = serializar_semaforo(p_semaforo);

	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_umc, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(header);
	free(p_semaforo);
	free(p_buffer);
}
