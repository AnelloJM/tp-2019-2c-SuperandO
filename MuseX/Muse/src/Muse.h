/*
 * Muse.h
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <Conexiones/Conexiones.h>
#include <SerializacionMuse/Serializacion.h>
#include <pthread.h>


t_log *logger;
t_config *config;
uint32_t memory_size;
uint32_t page_size;
uint32_t swap_size;
char* puerto;//no me gusta que sea global, pero funca

/*:::Estructuras:::*/

t_list* procesos_conectados;
t_bitarray *bitmap_para_frames;
t_bitarray *bitmap_para_swap;

typedef struct {
	uint32_t numero_de_frame;//en realidad tendria que ser el indice (todavia lo puedo cambiar)
	uint32_t numero_de_pagina;
	uint32_t numero_de_segmento;
	char* Id_del_proceso;
}Elemento_de_mi_tabla_de_frames_por_pagina;

/*
 * Esta es gobal, y me suena que esta medio al pedo,
 * por que el pedido me va a llegar de un solo
 * proceso,de ahi puedo buscar en sus tablas y ver si esta
 * y reemplazar en base a esa que a ver que onda
 */
t_list* tabla_de_frames_por_pagina;

typedef struct {
	bool tipo_de_segmento;//0:heap,1:map
	uint32_t numero_de_segmento;
}Segmento;

typedef struct {
	uint32_t base_logica;
	uint32_t tamanio_del_segmento;
	uint32_t puntero_a_tabla_de_paginas_del_segmento;//el numero seria el indice de en la tabla de paginas
}Elemento_de_mi_tabla_de_segmentos;

typedef struct {
	uint32_t numero_de_pagina;
	uint32_t numero_de_frame;
	bool prencia;
	bool modificado;//netamente para el algoritmo, total si o si cuando hago el swap tengo que bajar la pagina
}Elemento_de_mi_tabla_de_pagina_por_segmento;

typedef struct {
	char* Id_del_proceso;
	t_list* segmentos_del_proceso;//guarda Segmentos
	t_list* tabla_de_segmentos;
	t_list* tabla_de_pagina_por_segmento;
}Proceso;

/*Funciones iniciales*/
void iniciar_logger();
void iniciar_config();

/*Funciones*/
void *funcionMagica(int cliente);
char* obtener_IP(int sock_cliente);
void proceso_nuevo(char* id_del_proceso);
void iniciar_bitmap_de_frames();
void iniciar_bitmap_de_swap();

#endif /* MUSE_H_ */
