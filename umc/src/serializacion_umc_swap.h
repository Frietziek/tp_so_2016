/*
 * serializacion_umc_swap.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_UMC_H_
#define SERIALIZACION_UMC_H_

#include <serializacion.h>

typedef struct {
	int id_programa;
} t_programa;

typedef struct {
	int id_programa;
	int paginas_requeridas;
} t_programa_completo;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_pagina;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int valor;
} t_pagina_completa;

t_buffer *serializar_inicio_programa(t_programa_completo *inicio_programa);
void deserializar_inicio_programa(void *buffer,
		t_programa_completo *inicio_programa);

t_buffer *serializar_leer_pagina(t_pagina *leer_pagina);
void deserializar_leer_pagina(void *buffer, t_pagina *leer_pagina);

t_buffer *serializar_escribir_pagina(t_pagina_completa *escribir_pagina);
void deserializar_escribir_pagina(void *buffer,
		t_pagina_completa *escribir_pagina);

t_buffer *serializar_finalizar_programa(t_programa *finalizar_programa);
void deserializar_finalizar_programa(void *buffer,
		t_programa *finalizar_programa);

#endif /* SERIALIZACION_UMC_H_ */
