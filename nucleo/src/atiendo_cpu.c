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

	free(paquete);

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

	t_variable *variable = malloc(sizeof(t_variable));
	deserializar_variable(buffer, variable);

	t_variable_completa *variable_completa = malloc(
			sizeof(t_variable_completa));
	variable_completa->nombre = variable->nombre;
	variable_completa->valor = obtener_variable_compartida(variable->nombre);

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_OBTENER_VALOR_COMPARTIDA;

	t_buffer *p_cpu = serializar_variable_completa(variable_completa);
	h_cpu->longitud_mensaje = p_cpu->longitud_buffer;

	if (enviar_buffer(socket_conexion, h_cpu, p_cpu)
			< sizeof(h_cpu) + p_cpu->longitud_buffer) {
		perror("Fallo al enviar Variable Compartida al CPU\n");
	}

	free(variable);
	free(variable_completa);
	free(h_cpu);
	free(p_cpu);

}

void atiendo_asignar_compartida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable_completa *variable = malloc(sizeof(t_variable_completa));
	deserializar_variable_completa(buffer, variable);

	asignar_variable_compartida(variable->nombre, variable->valor);

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_ASIGNAR_VARIABLE_COMPARTIDA;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta CPU\n");
	}

	free(variable);
	free(h_cpu);

}

void atiendo_imprimir(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_variable_valor *variable = malloc(sizeof(t_valor_variable));
	deserializar_variable_valor(buffer, variable);

	t_header *h_consola = malloc(sizeof(t_header));
	h_consola->id_proceso_emisor = PROCESO_NUCLEO;
	h_consola->id_proceso_receptor = PROCESO_CONSOLA;
	h_consola->id_mensaje = MENSAJE_IMPRIMIR;

	t_buffer *p_consola = serializar_variable_valor(variable);
	h_consola->longitud_mensaje = p_consola->longitud_buffer;

	if (enviar_buffer(devuelve_socket_consola(socket_conexion), h_consola,
			p_consola) < sizeof(h_consola) + p_consola->longitud_buffer) {
		perror("Fallo al enviar Imprimir a la Consola\n");
	}

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_IMPRIMIR;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta a CPU\n");
	}

	free(variable);
	free(h_consola);
	free(p_consola);
	free(h_cpu);

}

void atiendo_imprimir_texto(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_texto *texto = malloc(sizeof(t_texto));
	deserializar_texto(buffer, texto);

	t_header *h_consola = malloc(sizeof(t_header));
	h_consola->id_proceso_emisor = PROCESO_NUCLEO;
	h_consola->id_proceso_receptor = PROCESO_CONSOLA;
	h_consola->id_mensaje = MENSAJE_IMPRIMIR_TEXTO;

	t_buffer *p_consola = serializar_texto(texto);
	h_consola->longitud_mensaje = p_consola->longitud_buffer;

	if (enviar_buffer(devuelve_socket_consola(socket_conexion), h_consola,
			p_consola) < sizeof(h_consola) + p_consola->longitud_buffer) {
		perror("Fallo al enviar Imprimir a la Consola\n");
	}

	t_header *h_cpu = malloc(sizeof(t_header));
	h_cpu->id_proceso_emisor = PROCESO_NUCLEO;
	h_cpu->id_proceso_receptor = PROCESO_CPU;
	h_cpu->id_mensaje = RESPUESTA_IMPRIMIR_TEXTO;
	h_cpu->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_cpu) < sizeof(h_cpu)) {
		perror("Fallo al enviar respuesta a CPU\n");
	}

	free(texto);
	free(h_consola);
	free(p_consola);
	free(h_cpu);

}

void atiendo_entrada_salida(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_entrada_salida *entrada_salida = malloc(sizeof(t_entrada_salida));
	deserializar_entrada_salida(buffer, entrada_salida);

	bloquear_pcb_dispositivo(socket_conexion, entrada_salida->nombre_dispositivo,
			entrada_salida->tiempo);
	asignar_pcb(socket_conexion);

	free(entrada_salida);

}

void atiendo_wait(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	// Distinto de 0, sigue con rafaga
	if (wait_semaforo(semaforo->nombre)) {
		t_header *h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_SEGUI_RAFAGA;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Seguir rafaga al CPU\n");
		}

		free(h_semaforo);
	} else {
		bloquear_pcb_semaforo(semaforo->nombre);

		t_header *h_semaforo = malloc(sizeof(t_header));
		h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
		h_semaforo->id_proceso_receptor = PROCESO_CPU;
		h_semaforo->id_mensaje = RESPUESTA_WAIT;
		h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

		if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
			perror("Fallo al responder Wait al CPU\n");
		}

		free(h_semaforo);
	}

	free(semaforo);
}

void atiendo_signal(void *buffer, int socket_conexion,
		t_config_nucleo *configuracion) {

	t_semaforo *semaforo = malloc(sizeof(t_semaforo));
	deserializar_semaforo(buffer, semaforo);

	signal_semaforo(semaforo->nombre);

	t_header *h_semaforo = malloc(sizeof(t_header));
	h_semaforo->id_proceso_emisor = PROCESO_NUCLEO;
	h_semaforo->id_proceso_receptor = PROCESO_CPU;
	h_semaforo->id_mensaje = RESPUESTA_SIGNAL;
	h_semaforo->longitud_mensaje = PAYLOAD_VACIO;

	if (enviar_header(socket_conexion, h_semaforo) < sizeof(h_semaforo)) {
		perror("Fallo al responder Signal al CPU\n");
	}

	free(semaforo);
}

void atiendo_quantum(void *buffer, int socket_conexion) {
	// Recibo PCB

	asignar_pcb(socket_conexion);
}

void atiendo_programa_finalizado(void *buffer, int socket_conexion) {
	// Recibo PCB

	asignar_pcb(socket_conexion);
}

t_buffer *serializar_variable(t_variable *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_variable(void *buffer, t_variable *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);

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

t_buffer *serializar_variable_completa(t_variable_completa *variable) {
	int cantidad_a_reservar = sizeof(int) + strlen(variable->nombre)
			+ sizeof(variable->valor);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, variable->nombre, &posicion_buffer);
	copiar_int_en_buffer(buffer, variable->valor, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_variable_completa(void *buffer, t_variable_completa *variable) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(variable->nombre),
			&posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(variable->valor),
			&posicion_buffer);

}

t_buffer *serializar_texto(t_texto *texto) {
	int cantidad_a_reservar = sizeof(int) + strlen(texto->texto);
	void *buffer = malloc(cantidad_a_reservar);

	int posicion_buffer = 0;

	copiar_string_en_buffer(buffer, texto->texto, &posicion_buffer);

	t_buffer *estructura_buffer = malloc(sizeof(t_buffer));
	estructura_buffer->contenido_buffer = buffer;
	estructura_buffer->longitud_buffer = posicion_buffer;

	return (estructura_buffer);
}

void deserializar_texto(void *buffer, t_texto *texto) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(texto->texto),
			&posicion_buffer);

}

void deserializar_entrada_salida(void *buffer, t_entrada_salida *entrada_salida) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer,
			&(entrada_salida->nombre_dispositivo), &posicion_buffer);
	escribir_atributo_desde_int_de_buffer(buffer, &(entrada_salida->tiempo),
			&posicion_buffer);

}

void deserializar_semaforo(void *buffer, t_semaforo *entrada_salida) {
	int posicion_buffer = 0;

	escribir_atributo_desde_string_de_buffer(buffer, &(entrada_salida->nombre),
			&posicion_buffer);

}

int obtener_variable_compartida(char *nombre_variable_compartida) {
	return 1;
}

int asignar_variable_compartida(char *nombre_variable_compartida, int valor) {
	return 1;
}

int devuelve_socket_consola(int socket_cpu) {
	return 1;
}

void bloquear_pcb_dispositivo(int socket_cpu, char *nombre_dispositivo, int tiempo) {
}

void bloquear_pcb_semaforo(char *nombre_semaforo) {
}

void asignar_pcb(int socket_cpu) {
}

int wait_semaforo(char *semaforo_nombre) {
	return 1;
}

void signal_semaforo(char *semaforo_nombre) {
}
