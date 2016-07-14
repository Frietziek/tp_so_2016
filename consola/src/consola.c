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
#include <commons/log.h>
#include <commons/string.h>

int socket_nucleo;
t_log *loggerManager;

int main(int argc, char **argv) {

	loggerManager = log_create("consola.log","CONSOLA", true, LOG_LEVEL_TRACE); //Se crea el log

	/*--------------------------------------------- CONFIGURACIONES ---------------------------------------------*/
	t_config_consola *configuracion_consola = malloc(sizeof(t_config_consola));
	cargar_configuracion_consola("config.consola.ini", configuracion_consola);
	log_trace(loggerManager, "Se cargaron las configuraciones de la consola con los siguientes valores: \nIP_NUCLEO=%s \nPUERTO_NUCLEO=%i\nNOMBRE_SCRIPT=%s\n", configuracion_consola->ip_nucleo, configuracion_consola->puerto_nucleo, configuracion_consola->nombre_script);
	/*--------------------------------------------------------------------------------------------------------------*/

	/*--------------------------------------------- CONEXIÓN NUCLEO -----------------------------------------------*/
	socket_nucleo = conectar_servidor(configuracion_consola->ip_nucleo, configuracion_consola->puerto_nucleo, &atender_nucleo);
	if(socket_nucleo != -1)
		log_trace(loggerManager, "Se establecio conexion con el nucleo, el socket es: %i", socket_nucleo);
	else
		log_error(loggerManager, "Ocurrio un problema al conectar con el nucleo (socket_nucleo: %i)", socket_nucleo);
	/*--------------------------------------------------------------------------------------------------------------*/

	signal(SIGINT, avisar_nucleo_de_terminacion_programa); //Control de CTRL + C

	/*--------------------------------------------- SCRIPT ANSISOP -----------------------------------------------*/
	char* path_del_script = string_from_format("../scripts-ansisop/scripts/%s", configuracion_consola->nombre_script);
	FILE *archivo_script = fopen(path_del_script, "r");

	if (archivo_script == NULL)
		log_error(loggerManager, "El archivo de script no existe (path: %s)", path_del_script);
	else
		log_trace(loggerManager, "El archivo de script existe y se ha abierto correctamente (path: %s)", path_del_script);

	enviar_codigo_al_nucleo(archivo_script, socket_nucleo);
	/*-------------------------------------------------------------------------------------------------------------*/

	getchar(); //pausa

	fclose(archivo_script);
	close(socket_nucleo);
	free(configuracion_consola);

	return EXIT_SUCCESS;
}

void cargar_configuracion_consola(char *archivo, t_config_consola *configuracion_consola) {

	t_config *configuracion = malloc(sizeof(t_config));
	configuracion = config_create(archivo);

	if (config_has_property(configuracion, "PUERTO_NUCLEO")) {
		configuracion_consola->puerto_nucleo = config_get_int_value(configuracion, "PUERTO_NUCLEO");
	} else {
		configuracion_consola->puerto_nucleo = DEF_PUERTO_NUCLEO;
	}

	if (config_has_property(configuracion, "IP_NUCLEO")) {
		configuracion_consola->ip_nucleo = config_get_string_value(configuracion, "IP_NUCLEO");
	} else {
		configuracion_consola->ip_nucleo = DEF_IP_NUCLEO;
	}

	if (config_has_property(configuracion, "NOMBRE_SCRIPT")) {
		configuracion_consola->nombre_script = config_get_string_value(configuracion, "NOMBRE_SCRIPT");
	} else {
		configuracion_consola->nombre_script = DEF_NOMBRE_SCRIPT;
	}


	free(configuracion);
}

void enviar_codigo_al_nucleo(FILE * archivo, int socket_nucleo) {
	log_trace(loggerManager, "Se esta procediento a enviar el codigo fuente al nucleo...");
	t_header *header = malloc(sizeof(t_header));

	char *Aux_Archivo[100];
	int Largo_Mensaje;

	fseek(archivo, 0, SEEK_END);
	Largo_Mensaje = ftell(archivo);
	rewind(archivo);

	t_texto *buffer = malloc(sizeof(t_texto));
	buffer->texto = malloc(Largo_Mensaje);
	*buffer->texto = NULL;

	while (feof(archivo) == 0) {
		fgets(Aux_Archivo, 100, archivo);
		strcat(buffer->texto, Aux_Archivo);
	}

	log_trace(loggerManager, "El codigo fuente del script a enviar es: \n%s \n", buffer->texto);

	t_buffer *p_buffer = serializar_imprimir_texto(buffer);

	header->id_proceso_emisor = PROCESO_CONSOLA;
	header->id_proceso_receptor = PROCESO_NUCLEO;
	header->id_mensaje = MENSAJE_INICIAR_PROGRAMA;
	header->longitud_mensaje = p_buffer->longitud_buffer;

	if (enviar_buffer(socket_nucleo, header, p_buffer)< sizeof(t_header) + p_buffer->longitud_buffer) {
		perror("Fallo enviar buffer");
	}
	free(buffer);
	free(header);
}

// Función para atender al nucleo
void atender_nucleo(t_paquete *paquete, int socket_conexion) {
	int id_mensaje = paquete->header->id_mensaje;

	switch (id_mensaje) {

	case MENSAJE_IMPRIMIR:;
		t_variable_valor * valor_imprimir = malloc(sizeof(t_variable_valor));
		deserializar_variable_valor(paquete->payload, valor_imprimir);
		consola_nucleo(socket_nucleo, RESPUESTA_IMPRIMIR);
		log_trace(loggerManager, "[Mensaje nucleo] MENSAJE_IMPRIMIR: %i", valor_imprimir->valor);
		break;
	case MENSAJE_IMPRIMIR_TEXTO:;
		t_texto * texto_imprimir = malloc(sizeof(t_texto));
		deserializar_texto(paquete->payload, texto_imprimir);
		consola_nucleo(socket_nucleo, RESPUESTA_IMPRIMIR_TEXTO);
		log_trace(loggerManager, "[Mensaje nucleo] MENSAJE_IMPRIMIR_TEXTO: %s", texto_imprimir->texto);
		break;
	case MENSAJE_PROGRAMA_FINALIZADO:
		log_trace(loggerManager, "[Mensaje nucleo] El nucleo solicita finalizar el programa");
		consola_nucleo(socket_nucleo, RESPUESTA_PROGRAMA_FINALIZADO);
		getchar();//Pausa antes de cerrar la consola
		exit(1);
	default:
		log_error(loggerManager, "[Mensaje nucleo] Mensaje no reconocido :(");
		break;
	}

}

//TODO: Revisar bien esta función, antes le enviaba el mensaje MENSAJE_INICIAR_PROGRAMA al nucleo, me hacían ruido bastantes cosas, borré cosas que me parecían estaban de más ojo
void avisar_nucleo_de_terminacion_programa() {
	log_trace(loggerManager, "Se ha presionado CTRL + C, se esta dando el aviso correspondiente al nucleo");

	signal(SIGINT, SIG_IGN); //TODO: Esto está de más? Por las dudas por ahora se deja
	consola_nucleo(socket_nucleo, MENSAJE_MATAR_PROGRAMA);
	pid_t pid = getpid();
	kill(pid, SIGTERM);
}

//TODO: Esta función no se está utilizando, analizar después si sirve
/*int Generar_Buffer_Programa(FILE *archivo, char **Aux_Buff) {
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
}*/

