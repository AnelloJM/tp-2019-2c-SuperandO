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
#include <Lista/lista.h>
#include <SerializacionMuse/Serializacion.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#define HEAP 0
#define MAP 1

t_log *logger;
t_config *config;


/*:::Estructuras de Frame:::*/

sem_t mutex_bitmap_frame;

uint32_t puntero_de_clock;

/*
 * Esta es gobal, y me suena que esta medio al pedo,
 * por que el pedido me va a llegar de un solo
 * proceso,de ahi puedo buscar en sus tablas y ver si esta
 * y reemplazar en base a esa que a ver que onda
 */
t_list* tabla_de_frames_por_pagina;

t_bitarray *bitmap_para_frames;


typedef struct {
	uint32_t numero_de_frame;//en realidad tendria que ser el indice (todavia lo puedo cambiar)
	uint32_t numero_de_pagina;
	uint32_t numero_de_segmento;
	char* Id_del_proceso;
}Elemento_de_mi_tabla_global_de_frames_por_pagina;

/*:::Estructuras Basica:::*/

void* UPCM;
uint32_t memory_size;
uint32_t page_size;
char* puerto;//no me gusta que sea global, pero funca



t_list* procesos_conectados;

typedef struct {
	bool esta_libre;//1:libre,0:usado
	uint32_t tamanio;
}Heap_de_metadata_de_segmento;


typedef struct {
	bool tipo_de_segmento;//0:heap,1:map
	uint32_t numero_de_segmento;
}Segmento;

typedef struct {
	uint32_t numero_de_segmento;
	uint32_t base_logica;
	uint32_t tamanio_del_segmento;
	uint32_t puntero_a_tabla_de_paginas_del_segmento;//id_de_tabla
}Elemento_de_mi_tabla_de_segmentos;

typedef struct {
	uint32_t id_de_tabla;
	uint32_t numero_de_pagina;
	uint32_t numero_de_frame;
	bool prencia;//igual de uso
	bool uso;
	bool modificado;//netamente para el algoritmo, total si o si cuando hago el swap tengo que bajar la pagina
}Elemento_de_mi_tabla_de_pagina_por_segmento;

typedef struct {
	char* Id_del_proceso;
	uint32_t ultimo_id_de_tabla_de_pagina;
	t_list* segmentos_del_proceso;//guarda Segmentos
	t_list* tabla_de_segmentos;
	t_list* tabla_de_pagina_por_segmento;
}Proceso;


/*:::Estructuras de Swap:::*/

uint32_t swap_size;
t_list* tabla_de_swap;
t_bitarray *bitmap_para_swap;
void* archivo_de_swap;

typedef struct {
	char* Id_del_proceso;
	uint32_t numero_de_pagina;
	uint32_t posicion_en_swap;
}Elemento_de_tabla_de_swap;

/*Funciones iniciales*/
void iniciar_logger();
void iniciar_config();

/*Funciones*/
void *funcionMagica(int cliente);
char* obtener_Id_real(int sock_cliente,char* id);
void proceso_nuevo(char* id_del_proceso);
void iniciar_bitmap_de_frames();
void iniciar_bitmap_de_swap();

#endif /* MUSE_H_ */
