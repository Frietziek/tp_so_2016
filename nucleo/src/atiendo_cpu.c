/*
 * atiendo_cpu.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "atiendo_cpu.h"

void atender_cpu(t_paquete *paquete, int socket_cpu,
		t_config_nucleo *configuracion) {

	switch (paquete->header->id_mensaje) {
	case MENSAJE_HANDSHAKE:
		atiendo_handshake(paquete->payload, socket_cpu);
		printf("Se establecio conexion con cpu\n\n");
		break;
	case MENSAJE_OBTENER_VALOR_COMPARTIDA:
		atiendo_obtener_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ASIGNAR_VARIABLE_COMPARTIDA:
		atiendo_asignar_compartida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_IMPRIMIR:
		atiendo_imprimir(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_IMPRIMIR_TEXTO:
		atiendo_imprimir_texto(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_ENTRADA_SALIDA:
		atiendo_entrada_salida(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_WAIT:
		atiendo_wait(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_SIGNAL:
		atiendo_signal(paquete->payload, socket_cpu, configuracion);
		break;
	case MENSAJE_QUANTUM:
		atiendo_quantum(paquete->payload, socket_cpu);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		atiendo_programa_finalizado(paquete->payload, socket_cpu);
		break;
	}

}

void atiendo_handshake(void *buffer, int socket_conexion) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_NUCLEO;
	header->id_proceso_receptor = PROCESO_CPU;
	header->id_mensaje = REPUESTA_HANDSHAKE;
	header->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, header) < sizeof(header)) {
		perror("Fallo al enviar confirmacion Handshake\n");
	}

	free(header);
}

void atiendo_obtener_compartida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_asignar_compartida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_imprimir(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable_valor *variable = malloc(sizeof(t_valor_variable));
	deserializar_variable_valor(buffer, variable);

	t_header *header_consola = malloc(sizeof(t_header));
	header_consola->id_proceso_emisor = PROCESO_NUCLEO;
	header_consola->id_proceso_receptor = PROCESO_CONSOLA;
	header_consola->id_mensaje = MENSAJE_IMPRIMIR;

	t_buffer *payload_consola = serializar_variable_valor(variable);
	header_consola->longitud_mensaje = payload_consola->longitud_buffer;

	// TODO Enviar a la consola para que imprima
	// TODO Responder a CPU

}

void atiendo_imprimir_texto(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_entrada_salida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_wait(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_signal(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {
}

void atiendo_quantum(void *buffer, int socket_conexion) {
}

void atiendo_programa_finalizado(void *buffer, int socket_conexion) {
}

t_buffer *serializar_variable_valor(t_variable_valor *variable) {
	int cantidad_a_reservar = sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_variable_valor(void *buffer, t_variable_valor *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);

}
