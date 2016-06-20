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

int aux_socket_nucleo;

void contar();

int main(int argc, char **argv) {

	FILE *archivo;
	char *buffer, **Aux_Buff;
	int Largo_Buffer;
	struct sigaction sa;
	t_config_consola *configuracion = malloc(sizeof(t_config_consola)); // Estructura de configuracion de la consola
	cargaConfiguracionConsola("src/config.consola.ini", configuracion);
	int socket_nucleo = conectar_servidor(configuracion->ip,
			configuracion->puerto, &atender_nucleo);
	aux_socket_nucleo = socket_nucleo;

	signal(SIGINT, contar);

	printf("Proceso Consola creado.\n");

	if (socket_nucleo > 0) {
		printf("Consola conectada con el Núcleo.\n");
		//handshake_consola_nucleo(socket_nucleo);
	} else {
		perror("Error al conectarse con la Núcleo\n");
	}

	// Lee Archivo y envia el archivo Ansisop
	//archivo = fopen(argv[1], "r"); //El argv[1] tiene la direccion del archivo Ansisop
	archivo = fopen("./Programa", "r");

	if (archivo == NULL)
		exit(1);

	enviar_codigo(archivo, socket_nucleo);

	while (1)
		;

	fclose(archivo);
	close(socket_nucleo);
	free(configuracion);
	free(buffer);
	return EXIT_SUCCESS;
}

void cargaConfiguracionConsola(char *archivo,
		t_config_consola *configuracionConsola) {
	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);
	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracionConsola->puerto = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
	} else {
		configuracionConsola->puerto = DEF_PUERTO_Nucleo;
	}
	if (config_has_property(configuracion, "IP_NUCLEO")) {
		configuracionConsola->ip = config_get_string_value(configuracion,
				"IP_NUCLEO");
	} else {
		configuracionConsola->ip = DEF_IP_Nucleo;
	}
	free(configuracion);
}

int Generar_Buffer_Programa(FILE *archivo, char **Aux_Buff) {
	char Aux_Archivo[100];
	int len, aux_len = 0;
	char * buffer;

	fseek(archivo, 0, SEEK_END);
	len = ftell(archivo);
	rewind(archivo);

	buffer = malloc(len);
	*buffer = NULL;

	while (feof(archivo) == 0) {

		fgets(Aux_Archivo, 100, archivo);

		strcat(buffer, Aux_Archivo);
		//strcpy (Aux_buffer,buffer);

	}

	printf("\n\n\n\nEl Codigo linea: %s \n\n\n\n", buffer);
	*Aux_Buff = buffer;

	return (len);
}

void enviar_codigo(FILE * archivo, int socket_nucleo) {
	t_header *header = malloc(sizeof(t_header));
	t_buffer *p_buffer;

	char *Aux_Archivo[100];
	int Largo_Mensaje;

	fseek(archivo, 0, SEEK_END);
	Largo_Mensaje = ftell(archivo);
	rewind(archivo);

	t_texto *buffer = malloc(sizeof(t_texto));
	buffer->texto = malloc(Largo_Mensaje);
	*buffer->texto = NULL;
	//buffer->texto = malloc(Largo_Mensaje);

	while (feof(archivo) == 0) {
		fgets(Aux_Archivo, 100, archivo);

		strcat(buffer->texto, Aux_Archivo);

	}

	printf("El Codigo linea: %s \n", buffer->texto);

	p_buffer = serializar_imprimir_texto(buffer);

	header->id_proceso_emisor = PROCESO_CONSOLA;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_INICIAR_PROGRAMA;
	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)
			< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}
	free(buffer);
	free(header);
}

// Funciones CPU - Nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion) {
	int opcion;
	switch (paquete->header->id_mensaje) {
	case MENSAJE_IMPRIMIR:
		printf("La variable vale:\n");
		t_variable_valor * valor_imprimir = malloc(sizeof(t_variable_valor));
		deserializar_variable_valor(paquete->payload, valor_imprimir);
		printf("%d\n", valor_imprimir->valor);
		opcion = RESPUESTA_IMPRIMIR;
		consola_nucleo(aux_socket_nucleo, opcion);
		break;
	case MENSAJE_IMPRIMIR_TEXTO:
		printf("El texto es:\n");
		t_texto * texto_imprimir = malloc(sizeof(t_texto));
		deserializar_texto(paquete->payload, texto_imprimir);
		printf("%s\n", texto_imprimir->texto);
		opcion = RESPUESTA_IMPRIMIR_TEXTO;
		consola_nucleo(aux_socket_nucleo, opcion);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		printf("Programa Finalizado Correctamente\n");
		opcion = RESPUESTA_PROGRAMA_FINALIZADO;
		consola_nucleo(aux_socket_nucleo, opcion);
		exit(1);
	default:
		printf("Comando no reconocido\n");
		break;
	}
}

void contar() {
	/* Primero desactivamos la señal SIGINT por si se pulsa CTRL+C
	 mientras se está ejecutando esta función. */
	int opcion = MENSAJE_INICIAR_PROGRAMA;
	pid_t pid;
	signal(SIGINT, SIG_IGN);
	printf("Has pulsado CTRL-C\n");
	printf("llego");
	consola_nucleo(aux_socket_nucleo, opcion);
	pid = getpid();
	kill(pid, SIGTERM);
}
/*
 void handler_señal(int s) {
 int opcion = MENSAJE_MATAR_PROCESO;
 pid_t pid;
 consola_nucleo(aux_socket_nucleo, opcion);
 printf("llego");
 pid = getpid ();
 kill (pid,SIGTERM);
 printf("No llego");
 }

 */
