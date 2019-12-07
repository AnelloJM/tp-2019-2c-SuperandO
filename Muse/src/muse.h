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
#include "servidor-cliente/lib_conexion.h" //esta libreria se define asi porque está en nuestro workspace
#include "servidor-cliente/conexion_muse.c" // esta la sigo poniendo por mi editor, ignorenla
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>


//Varibles globales



t_log* logger;
t_config* archivoConfig;


void *UPCM; //UPCM = unica porcion contigua de memoria
t_list *tabla_de_frames; //tabla de frames la tomamos como una lista de 0/1

int memory_size;
int page_size;
int swap_size;
int frames_table_size;
int id;
char* puerto;
char* ip;
int socketMuse;
int socket_cliente;


//estructuras

struct Pagina
{
  int bit_presencia;
  int numero_frame_asociado;
  int numero_pagina;
  struct Pagina *next_pagina;
};


typedef struct
{
  int size;
  bool isFree;
}Heap;


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
void poner_heap(Heap *heap,uint32_t posicion);
uint32_t calcular_posicion_en_UPCM(uint32_t n_frame);
uint32_t buscar_frame_libre();
void reservar_espacio(uint32_t posicion,uint32_t tamanio);
uint32_t frame_free_size(uint32_t posicion);
uint32_t free_frame_heap(uint32_t posicion);
void alloc_tam(uint32_t tam,uint32_t posicion);
void mostrar_frames_table();
uint32_t buscar_proceso(uint32_t id_proceso);


#endif
