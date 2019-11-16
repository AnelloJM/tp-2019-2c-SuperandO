/*
 * Sac-Server.h
 *
 *  Created on: 15 nov. 2019
 *      Author: utnso
 */

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <Conexiones/Conexiones.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <pthread.h>


#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_


//Estructuras para FileSystem

typedef uint32_t ptrGBloque;

int cantidad_de_bloques;

typedef struct {
	unsigned char bytes [4096];
} Bloque;

typedef struct {
	char identificador [3];
	uint32_t version;
	ptrGBloque inicio_bitmap;
	uint32_t tamanio_bitmap;
	char rellenuto[4081];
}Header;

typedef struct{
	char *bitArray;
}Bitmap;

typedef struct{
	char dia[2];
	char mes[2];
	char anio[4];
}Fecha;


typedef struct{
	ptrGBloque  direcciones_del_bloque_de_datos [1024];
}Bloque_indirecto_simple;

typedef struct {
	char estado;
	char nombre_del_archivo[71];
	ptrGBloque  padre;
	uint32_t tamanio_del_archivo;
	Fecha creacion;
	Fecha modificado;
	ptrGBloque  array_de_punteros [1000];
} Nodo;

typedef struct {
	Nodo nodos[1024];
} Tabla_de_nodos;


void iniciar_Sac_Server(Header *header, Bitmap* bitmap, Tabla_de_nodos *tabla_de_nodos);
void iniciar_header(Header *header);
t_bitarray 	*iniciar_bitmap(Bitmap* bitmap);

#endif /* SAC_SERVER_H_ */
