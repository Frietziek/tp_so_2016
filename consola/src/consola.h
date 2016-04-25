/*
 * consola.h
 *
 *  Created on: 24/4/2016
 *      Author: utnso
 */

#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

// Valores por defecto de la configuracion
#define DEF_PUERTO_Nucleo 3605 // Puerto para recibir conexiones (socket host)
#define DEF_IP_Nucleo "10.0.0.100" // Ip del proceso Nucleo (socket Server)


typedef struct {
	int puerto;
	char *ip;
} t_config_consola;

void cargaConfiguracionConsola(char *archivo, t_config_consola *configuracion);


#endif /* SRC_CONSOLA_H_ */
