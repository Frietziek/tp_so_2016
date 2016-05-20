/*
 * serializacion_umc_swap.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_UMC_H_
#define SERIALIZACION_UMC_H_

typedef struct {
	int id_programa;
	int paginas_requeridas;
} t_inicio_programa;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_leer_pagina;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	void *buffer;
} t_escribir_pagina;

typedef struct {
	int id_programa;
} t_fin_programa;

t_buffer *serializar_inicio_programa(t_inicio_programa *inicio_programa);
void deserializar_inicio_programa(void *buffer,
		t_inicio_programa *inicio_programa);

t_buffer *serializar_leer_pagina(t_leer_pagina *leer_pagina);
void deserializar_leer_pagina(void *buffer, t_leer_pagina *leer_pagina);

t_buffer *serializar_escribir_pagina(t_escribir_pagina *escribir_pagina);
void deserializar_escribir_pagina(void *buffer,
		t_escribir_pagina *escribir_pagina);

t_buffer *serializar_finalizar_programa(
		t_fin_programa *finalizar_programa);
void deserializar_finalizar_programa(void *buffer,
		t_fin_programa *finalizar_programa);

#endif /* SERIALIZACION_UMC_H_ */
