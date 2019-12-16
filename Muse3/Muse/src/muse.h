/*
 * muse.h
 *
 *  Created on: Dec 12, 2019
 *      Author: cruz636
 */

#ifndef MUSE_H_
#define MUSE_H_

#include <stdint.h>
#include <stddef.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <readline/readline.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include "conexion_muse.h"

//Variables globales

t_log* logger;
t_config* archivoConfig;

/* UPCM = unica porcion contigua de memoria
 *
 *
 * La UPCM guarda las paginas.
 * Para mi para que guarde el dato real tien que ser un vector de Paginas nuevas
 * Pagina_nueva UPCM [memory_size/tamanio_de_frame]; //ver de donde sacar el tamanio de frame
 */
void *UPCM;

/*tabla de frames la tomamos como una lista de 0/1
 *
 * ~>En realidad ahora tiene que ser de elementos de tabla de frame
*/
t_list *tabla_de_frames;
t_list *tabla_de_paginas_por_segmento;

typedef struct {
	char* pid;
	uint32_t numero_de_pagina;
}Elemento_de_tabla_de_frame;

typedef struct {
	char* pid;
	uint32_t numero_de_segmento;
	uint32_t numero_de_pagina;
	uint32_t bit_de_presencia;
	uint32_t bit_de_modificado;
}Elemento_de_tabla_de_paginas_por_segmento;

/*
 * Es un char* para hacer malloc(tam_pagina).
 */
typedef struct {
	char* datos_de_pagina;
}Pagina_nueva;

//El segmento va a ser un puntero en donde se haga un memcpy de las paginas de ese segmento

typedef struct
{
  bool isFree;
  int size;
}Heap; //se tiene que llamar HeapMetadata en realidad, hay que cambiarlo en todos lados


int memory_size;
int page_size;
int swap_size;
int frames_table_size;
int id;
char* puerto;
char* ip;
int socketMuse;
int socket_cliente;


//estructuras cruz

typedef struct
{
  int tam_libre;
  int tam_usado;
  int bit_presencia;
  int numero_frame_asociado; //numero de marco de la UPCM
 }Pagina;

 typedef struct
 {
	 char *pid; // "ip:proceso_id"
	 t_list *Paginas;

 }Proceso;


t_list *tabla_de_procesos;


 //estructuras viejas
typedef struct
{
  int size;
  int base;
  struct Pagina *primer_pagina;

} Segmento;


//deberiamos pasarlos a uint32_t ??
void leerArchivoDeConfiguracion();
void setearValores(t_config* archivoConfig);
void crearLogger();
int iniciarLogger();
int recibir_peticion(uint32_t tam);
int pasar_a_frames(uint32_t am);
uint32_t tratar_muse_alloc(uint32_t tam,uint32_t id_proceso);
uint32_t tratar_muse_cpy(uint32_t tam,uint32_t posicion,void * data,uint32_t id_proceso);
uint32_t tratar_muse_free(uint32_t dir,uint32_t id_proceso);
void poner_heap(Heap *heap,uint32_t posicion);
uint32_t calcular_posicion_en_UPCM(uint32_t n_frame);
uint32_t buscar_frame_libre();
void reservar_espacio(uint32_t posicion,uint32_t tamanio);
uint32_t frame_free_size(uint32_t posicion);
uint32_t free_frame_heap(uint32_t posicion);
void alloc_tam(uint32_t tam,uint32_t posicion);
void mostrar_frames_table();
uint32_t buscar_proceso(uint32_t id_proceso);
void liberar_espacio(uint32_t posicion);
uint32_t free_size_in_frame(uint32_t frame,uint32_t tam);


#endif



