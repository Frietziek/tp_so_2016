/*
 * serializacion_nucle_consola.h
 *
 *  Created on: 22/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_NUCLEO_CONSOLA_H_
#define SERIALIZACION_NUCLEO_CONSOLA_H_

#include <serializacion.h>

#define CODIGO 0
#define FINALIZAR 1

void deserializar_codigo(void* buffer, char **codigo, int longitud_mensaje);

#endif /* SERIALIZACION_NUCLE_CONSOLA_H_ */
