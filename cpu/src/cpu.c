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
	printf("Proceso CPU creado.");

	socket_nucleo = conectar_servidor(configuracion->ip_nucleo,
			configuracion->puerto_nucleo);

	// INICIO EJEMPLO ENVIO PAQUETE SIN PAYLOAD

	t_header *header_handshake = malloc(sizeof(t_header));
	header_handshake->id_proceso_emisor = 2;
	header_handshake->id_proceso_receptor = 1;
	header_handshake->id_mensaje = 0;
	header_handshake->longitud_mensaje = 0;
	enviar_header(socket_nucleo, header_handshake);
	free(header_handshake);

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

	header->id_proceso_emisor = 2;
	header->id_proceso_receptor = 1;
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
	socket_umc = conectar_servidor(configuracion->ip_umc,
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

// Funciones CPU - UMC
void definir_variable(char *variable) {
	t_variable *p_variable = malloc(sizeof(t_variable));
	p_variable->nombre = variable;
	//t_buffer *p_buffer = serializar_variable(p_variable);
	// t_buffer *h_buffer; // TODO hacer el header
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_umc, buffer);
	free(p_variable);
}

void obtener_posicion_variable(char * variable) {
	t_variable *p_variable = malloc(sizeof(t_variable));
	p_variable->nombre = variable;
	//t_buffer *p_buffer = serializar_variable(p_variable);
	// t_buffer *h_buffer; // TODO hacer el header
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_umc, buffer);
	free(p_variable);
}

void dereferenciar(int pagina, int offset, int tamanio) {
	t_dereferenciar_variable *p_derefenciar = malloc(
			sizeof(t_dereferenciar_variable));
	p_derefenciar->pagina = pagina;
	p_derefenciar->offset = offset;
	p_derefenciar->tamanio = tamanio;
	t_buffer *p_buffer = serializar_dereferenciar(p_derefenciar);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_umc, buffer);
	free(p_derefenciar);
}

void asignar_variable(int pagina, int offset, int tamanio, int valor) {
	t_variable_en_memoria *p_asignar = malloc(sizeof(t_variable_en_memoria));
	p_asignar->pagina = pagina;
	p_asignar->offset = offset;
	p_asignar->tamanio = tamanio;
	p_asignar->valor = valor;
	t_buffer *p_buffer = serializar_asignar(p_asignar);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_umc, buffer);
	free(p_asignar);
}

// Funciones CPU - Nucleo
void obtener_valor_compartida(char *variable) {
	t_variable_compartida *p_compartida = malloc(sizeof(t_variable_compartida));
	p_compartida->nombre = variable;
	t_buffer *p_buffer = serializar_variable_compartida(p_compartida);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_compartida);
}

void asignar_valor_compartida(char *variable, int valor) {
	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;
	p_compartida->valor = valor;
	t_buffer *p_buffer = serializar_asignar_variable_compartida(p_compartida);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_compartida);
}

void imprimir_variable(char *variable, int valor) {
	t_variable *p_compartida = malloc(sizeof(t_variable));
	p_compartida->nombre = variable;
	p_compartida->valor = valor;
	t_buffer *p_buffer = serializar_imprimir_variable(p_compartida);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_compartida);
}

void imprimir_texto(char *texto) {
	t_texto *p_texto = malloc(sizeof(t_variable));
	p_texto->texto = texto;
	t_buffer *p_buffer = serializar_imprimir_texto(p_texto);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_texto);
}

void entrada_salida(char *nombre, int tiempo) {
	t_entrada_salida *p_entrada_salida = malloc(sizeof(t_entrada_salida));
	p_entrada_salida->nombre_dispositivo = nombre;
	p_entrada_salida->tiempo = tiempo;
	t_buffer *p_buffer = serializar_entrada_salida(p_entrada_salida);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_entrada_salida);
}

void wait_semaforo(char *semaforo) {
	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;
	t_buffer *p_buffer = serializar_semaforo(p_semaforo);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_semaforo);
}

void signal_semaforo(char *semaforo) {
	t_semaforo *p_semaforo = malloc(sizeof(t_semaforo));
	p_semaforo->nombre = semaforo;
	t_buffer *p_buffer = serializar_semaforo(p_semaforo);
	// t_buffer *h_buffer; // TODO hacer el header;
	t_buffer *buffer; // TODO Rellenar con header y payload
	enviar_mensaje(socket_nucleo, buffer);
	free(p_semaforo);
}
