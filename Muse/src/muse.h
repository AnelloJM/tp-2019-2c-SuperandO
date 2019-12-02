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
#include "../../ComunParaTodos/Conexiones/Conexiones.h" //esta libreria se define asi porque está en nuestro workspace
#include "../../ComunParaTodos/Conexiones/Conexiones.c" // esta la sigo poniendo por mi editor, ignorenla
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>


//Varibles globales

// reservamos bloque grande de memoria ( UPCM )
int *UPCM; //UPCM =
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
  bool isFree; //Bool isFree
}Heap;


typedef struct
{
  int size;
  int base;
  struct Pagina *primer_pagina;

} Segmento;

typedef struct
{
  int numero_frame;
  Heap espacio_ocupado;
  Heap espacio_free;
//  Frame *next_frame;
}Frame;




t_log* logger;
t_config* archivoConfig;



//deberiamos pasarlos a uint32_t ??
void leerArchivoDeConfiguracion();
void setearValores(t_config* archivoConfig);
void crearLogger();
int iniciarLogger();
int recibir_peticion(uint32_t tam);
int pasar_a_frames(tam);



#endif
