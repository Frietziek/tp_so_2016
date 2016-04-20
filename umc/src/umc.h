/*
 * umc.h
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#ifndef SRC_UMC_H_
#define SRC_UMC_H_

// Valores por defecto de la configuracion
#define DEF_PUERTO 3603 // Puerto para recibir conexiones (socket host)
#define DEF_IP_SWAP "10.0.0.100" // Ip del proceso Swap (socket client)
#define DEF_PUERTO_SWAP 3439 // Puerto del proceso Swap
#define DEF_MARCOS 10 // Marcos disponibles en el sistema
#define DEF_MARCO_SIZE 1024 // Tamanio de los marcos
#define DEF_MARCO_X_PROC 2 // Cantidad de marcos por programas AnSISOP
#define DEF_ENTRADAS_TLB 0 // Entradas en TLB
#define DEF_RETARDO 5000 // Retardo de respuesta en ms

typedef struct {
	int puerto;
	char *ip_swap;
	int puerto_swap;
	int marcos;
	int marco_size;
	int marco_x_proc;
	int entradas_tlb;
	int retardo;
} t_config_umc;

void cargaConfiguracionUMC(char *archivo, t_config_umc *configuracion);

#endif /* SRC_UMC_H_ */
