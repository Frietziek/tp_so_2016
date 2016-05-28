/*
 * serializacion_cpu_umc.h
 *
 *  Created on: 14/5/2016
 *      Author: utnso
 */

#ifndef SERIALIZACION_CPU_H_
#define SERIALIZACION_CPU_H_

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int socket_pedido;
} t_pagina;

typedef struct {
	int tamanio;
} t_pagina_tamanio;

typedef struct {
	int pagina;
	int offset;
	int tamanio;
	int valor;
	int socket_pedido;
} t_pagina_completa;

t_buffer *serializar_pagina(t_pagina *pagina);
void deserializar_pagina(void *buffer, t_pagina *pagina);

t_buffer *serializar_pagina_completa(t_pagina_completa *pagina);
void deserializar_pagina_completa(void *buffer, t_pagina_completa *pagina);

void deserializar_pagina_tamanio(void *buffer, t_pagina_tamanio *pagina_tamanio);

#endif /* SERIALIZACION_CPU_H_ */
