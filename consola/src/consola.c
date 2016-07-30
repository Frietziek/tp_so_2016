/*
 ============================================================================
 Name        : consola.c
 Author      : losmallocados
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "consola.h"

int socket_nucleo;
t_log *loggerManager;

int main(int argc, char *argv[]) {

	loggerManager = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_TRACE); //Se crea el log

	/*--------------------------------------------- CONFIGURACIONES ---------------------------------------------*/
	t_config_consola *configuracion_consola = malloc(sizeof(t_config_consola));
	configuracion_consola->ip_nucleo = malloc(40);
	configuracion_consola->nombre_script = malloc(40);

	char *path_configuracion;

	if (argv[1] != NULL) //Osea, si se ejecuto mediante el shebang
		path_configuracion = "/usr/bin/config.consola.ini";
	else
		path_configuracion = "config.consola.ini";

	cargar_configuracion_consola(path_configuracion, configuracion_consola);
	log_trace(loggerManager,
			"Se cargaron las configuraciones de la consola con los siguientes valores: \nIP_NUCLEO=%s \nPUERTO_NUCLEO=%i\nNOMBRE_SCRIPT=%s\n",
			configuracion_consola->ip_nucleo,
			configuracion_consola->puerto_nucleo,
			configuracion_consola->nombre_script);
	/*--------------------------------------------------------------------------------------------------------------*/

	sem_init(&s_consola_finaliza, 0, 0); // Semaforo para la finalizacion de la Consola

	/*--------------------------------------------- CONEXIÓN NUCLEO -----------------------------------------------*/
	socket_nucleo = conectar_servidor(configuracion_consola->ip_nucleo,
			configuracion_consola->puerto_nucleo, &atender_nucleo);
	if (socket_nucleo != -1)
		log_trace(loggerManager,
				"Se establecio conexion con el nucleo, el socket es: %i",
				socket_nucleo);
	else
		log_error(loggerManager,
				"Ocurrio un problema al conectar con el nucleo (socket_nucleo: %i)",
				socket_nucleo);

	enviar_handshake_al_nucleo(socket_nucleo);
	/*--------------------------------------------------------------------------------------------------------------*/

	signal(SIGINT, avisar_nucleo_de_terminacion_programa); //Control de CTRL + C

	/*--------------------------------------------- SCRIPT ANSISOP -----------------------------------------------*/
	char* path_del_script;

	if (argv[1] != NULL) //Osea, si se ejecuto mediante el shebang
		path_del_script = argv[1];
	else
		path_del_script = string_from_format("../scripts-ansisop/scripts/%s",
				configuracion_consola->nombre_script);

	FILE *archivo_script = fopen(path_del_script, "r");

	if (archivo_script == NULL)
		log_error(loggerManager, "El archivo de script no existe (path: %s)",
				path_del_script);
	else
		log_trace(loggerManager,
				"El archivo de script existe y se ha abierto correctamente (path: %s)",
				path_del_script);

	enviar_codigo_al_nucleo(archivo_script, socket_nucleo);

	if (argv[1] == NULL)
		free(path_del_script);
	/*-------------------------------------------------------------------------------------------------------------*/

	sem_wait(&s_consola_finaliza);

	/*----------------------------------------- Liberación de recursos --------------------------------------------*/
	fclose(archivo_script);
	close(socket_nucleo);
	free(configuracion_consola);
	free(configuracion_consola->ip_nucleo);
	free(configuracion_consola->nombre_script);
	/*-------------------------------------------------------------------------------------------------------------*/

	return EXIT_SUCCESS;
}

void cargar_configuracion_consola(char *archivo,
		t_config_consola *configuracion_consola) {

	t_config *configuracion = config_create(archivo);

	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracion_consola->puerto_nucleo = config_get_int_value(
				configuracion, "PUERTO_NUCLEO");
	} else {
		configuracion_consola->puerto_nucleo = DEF_PUERTO_NUCLEO;
	}

	if (config_has_property(configuracion, "IP_NUCLEO")) {
		strcpy(configuracion_consola->ip_nucleo,
				config_get_string_value(configuracion, "IP_NUCLEO"));
	} else {
		configuracion_consola->ip_nucleo = DEF_IP_NUCLEO;
	}

	if (config_has_property(configuracion, "NOMBRE_SCRIPT")) {
		strcpy(configuracion_consola->nombre_script,
				config_get_string_value(configuracion, "NOMBRE_SCRIPT"));
	} else {
		configuracion_consola->nombre_script = DEF_NOMBRE_SCRIPT;
	}

	config_destroy(configuracion);
}

void enviar_codigo_al_nucleo(FILE * archivo, int socket_nucleo) {
	log_trace(loggerManager,
			"Se esta procediento a enviar el codigo fuente al nucleo...");
	t_header *header = malloc(sizeof(t_header));

	fseek(archivo, 0, SEEK_END);
	int longitud_codigo = ftell(archivo);
	rewind(archivo);

	t_texto *buffer = malloc(sizeof(t_texto));
	buffer->texto = calloc(longitud_codigo, sizeof(char));

	fread(buffer->texto, sizeof(char), longitud_codigo, archivo);

	log_trace(loggerManager, "El codigo fuente del script a enviar es: \n%s \n",
			buffer->texto);

	t_buffer *p_buffer = serializar_imprimir_texto(buffer);

	header->id_proceso_emisor = PROCESO_CONSOLA;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_INICIAR_PROGRAMA;
	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}

	free(p_buffer->contenido_buffer);
	free(p_buffer);
	//free(buffer->texto);
	free(buffer);
	free(header);
}

// Función para atender al nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion) {
	int id_mensaje = paquete->header->id_mensaje;

	switch (id_mensaje) {

	case RESPUESTA_HANDSHAKE:
		log_trace(loggerManager, "[Mensaje nucleo] Handshake confirmado");
		break;

	case MENSAJE_IMPRIMIR:
		;
		t_variable_valor * valor_imprimir = malloc(sizeof(t_variable_valor));
		deserializar_variable_valor(paquete->payload, valor_imprimir);
		consola_nucleo(socket_nucleo, RESPUESTA_IMPRIMIR);
		log_trace(loggerManager, "[Mensaje nucleo] MENSAJE_IMPRIMIR: %i",
				valor_imprimir->valor);
		free(valor_imprimir);
		break;

	case MENSAJE_IMPRIMIR_TEXTO:
		;
		t_texto * texto_imprimir = malloc(sizeof(t_texto));
		deserializar_texto(paquete->payload, texto_imprimir);
		consola_nucleo(socket_nucleo, RESPUESTA_IMPRIMIR_TEXTO);
		log_trace(loggerManager, "[Mensaje nucleo] MENSAJE_IMPRIMIR_TEXTO: %s",
				texto_imprimir->texto);
		free(texto_imprimir->texto);
		free(texto_imprimir);
		break;
	case MENSAJE_ERROR_AL_INICIAR:
		log_trace(loggerManager,
				"[Mensaje nucleo] El nucleo no pudo iniciar el programa");
		consola_nucleo(socket_nucleo, RESPUESTA_PROGRAMA_FINALIZADO_CONSOLA);

		sem_post(&s_consola_finaliza);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		log_trace(loggerManager,
				"[Mensaje nucleo] El nucleo solicita finalizar el programa");
		consola_nucleo(socket_nucleo, RESPUESTA_PROGRAMA_FINALIZADO_CONSOLA);

		sem_post(&s_consola_finaliza);
		break;
	case ME_TENGO_QUE_CERRAR:
		log_trace(loggerManager,
				"[Mensaje nucleo] El nucleo solicita finalizar el programa por sigint cpu cuando estaba ejecutando");
		sem_post(&s_consola_finaliza);
		break;
	default:
		log_error(loggerManager, "[Mensaje nucleo] Mensaje no reconocido :( %i",
				id_mensaje);
		break;

	}

}

void avisar_nucleo_de_terminacion_programa() {
	log_trace(loggerManager, "Finaliza consola");
	consola_nucleo(socket_nucleo, MENSAJE_MATAR_PROGRAMA);
	sem_post(&s_consola_finaliza);
}

void enviar_handshake_al_nucleo(int socket_nucleo) {
	t_header *header = malloc(sizeof(t_header));
	header->id_proceso_emisor = PROCESO_CONSOLA;
	header->id_proceso_receptor = PROCESO_NUCLEO;

	header->id_mensaje = HANDSHAKE_NUCLEO;
	header->longitud_mensaje = 0;

	int cantidad_bytes_enviados = enviar_header(socket_nucleo, header);

	if (cantidad_bytes_enviados < sizeof(t_header))
		log_error(loggerManager,
				"[Comunicacion nucleo] Ocurrió un problema al enviar el handshake al nucleo");
	else
		log_trace(loggerManager,
				"[Comunicacion nucleo] Se realizo el envio del handshake al nucleo correctamente");

	free(header);

}
