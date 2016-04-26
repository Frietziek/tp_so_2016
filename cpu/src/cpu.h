/*
 * cpu.h
 *
 *  Created on: 26/4/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

// Valores por defecto de la configuracion
#define DEF_IP_NUCLEO "0.0.0.0" // Ip del proceso Swap (socket client)
#define DEF_PUERTO_NUCLEO 5000 // Puerto para recibir conexiones (socket host)
#define DEF_IP_UMC "0.0.0.0" // Ip del proceso Swap (socket client)
#define DEF_PUERTO_UMC 3603 // Puerto para recibir conexiones (socket host)

typedef struct {
	char *ip_nucleo;
	int puerto_nucleo;
	char *ip_umc;
	int puerto_umc;
} t_config_cpu;

void cargaConfiguracionCPU(char *archivo, t_config_cpu *configuracion);

#endif /* CPU_H_ */
