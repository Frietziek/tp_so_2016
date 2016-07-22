/*
 * serializacion_umc_swap.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_UMC_H_
#define SERIALIZACION_UMC_H_

#include <stdlib.h>
#include <string.h>

typedef struct {
	int id_programa;
} t_programa;

typedef struct {
	int id_programa;
	int paginas_requeridas;
	char *codigo;
} t_programa_completo;

typedef struct {
	int id_programa;
	int nro_pagina_inicial;
	char * contenido;
} t_programa_para_escritura;

typedef struct {
	int id_programa;
	int pagina;
	int offset;
	int tamanio;
	int socket_pedido;
} t_pagina;

typedef struct {
	int tamanio;
} t_pagina_tamanio;

typedef struct {
	int id_programa;
	int pagina;
	int offset;
	int tamanio;
	void *valor;
	int socket_pedido;
} t_pagina_completa;

typedef struct {
	int id_programa;
	int paginas_requeridas;
} t_programa_nuevo;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
} t_pagina_pedido;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	void *valor;
} t_pagina_pedido_completa;

t_buffer *serializar_programa(t_programa *programa);
void deserializar_programa(void *buffer, t_programa *programa);

t_buffer *serializar_programa_completo(t_programa_completo *programa);
void deserializar_programa_completo(void *buffer, t_programa_completo *programa);

t_buffer *serializar_pagina(t_pagina *pagina);
void deserializar_pagina(void *buffer, t_pagina *pagina);

t_buffer *serializar_pagina_completa(t_pagina_completa *pagina);
void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina);

t_buffer *serializar_pagina_tamanio(t_pagina_tamanio *pagina_tamanio);
void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio);

t_buffer *serializar_programa_nuevo(t_programa_nuevo *programa);
void deserializar_programa_nuevo(void *buffer, t_programa_nuevo *programa);

t_buffer *serializar_programa_para_escritura(
		t_programa_para_escritura *programa);
void deserializar_programa_para_escritura(void *buffer,
		t_programa_para_escritura *programa);

t_buffer *serializar_pagina_pedido(t_pagina_pedido *pagina);
void deserializar_pagina_pedido(void *buffer, t_pagina_pedido *pagina);

t_buffer *serializar_pagina_pedido_completa(t_pagina_pedido_completa *pagina);
void deserializar_pagina_pedido_completa(void *buffer,
		t_pagina_pedido_completa *pagina);

#endif /* SERIALIZACION_UMC_H_ */
