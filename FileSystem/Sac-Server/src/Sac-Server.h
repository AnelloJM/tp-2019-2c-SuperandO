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
#include <sys/time.h>
#include <sys/mman.h>
#include <math.h>


#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_


//Estructuras para FileSystem

typedef uint32_t ptrGBloque;


typedef struct {
	/*unsigned*/ char bytes [4096];
} Bloque;

typedef struct {
	char identificador [3];
	uint32_t version;
	ptrGBloque inicio_bitmap;
	uint32_t tamanio_bitmap;
	unsigned char rellenuto[4081];
}Header;

typedef struct{
	char *bitArray;
}Bitmap;

typedef struct {
	char estado;//0: Borrado, 1: Ocupado, 2: Directorio
	char nombre_del_archivo[71];
	ptrGBloque  padre;
	uint32_t tamanio_del_archivo;
	uint64_t creacion;
	uint64_t modificado;
	ptrGBloque array_de_punteros[1000];
} Nodo;

typedef struct {
	Nodo nodos[1024];
} Tabla_de_nodos;

/////////////////////////////////
// FUNCIONES DE INICIALIZACION //
////////////////////////////////

void iniciar_Sac_Server();
void iniciar_header();
void cargar_bitmap(int cantidad);
void iniciar_tabla_de_nodos();

///////////////////////////////
// FUNCIONES ADMINISTRATIVAS //
//////////////////////////////

uint64_t timestamp();
void* funcionMagica(int cliente);
uint32_t tamanio_archivo(char *archivo);
int tamanio_archivo_en_bloques(uint32_t tamanio);

//////////////////////////
// FUNCIONES DEL BITMAP //
/////////////////////////

int buscar_espacio_en_bitmap();
void ocupar_bloque_en_bitmap(int indice);
void liberar_bloque_en_bitmap(int indice);

////////////////////////////////////
// FUNCIONES DE LA TABLA DE NODOS //
///////////////////////////////////

char** hallar_padres(char* nombre_buscado);
bool existe_path(char* path);
int buscar_nodo_libre();
char* obtener_nombre_padre_nodo(uint32_t numero_de_nodo);
char* obtener_nombre_nodo(uint32_t numero_de_nodo);
void crear_directorio_en_nodo(int numero_de_nodo, char *nombre_de_archivo);

///////////////////////
// FUNCIONES DE FUSE //
//////////////////////

uint32_t Hacer_Getattr(char *path);
uint32_t Hacer_ReadDir(char *path);
uint32_t Hacer_Open(char *path);
uint32_t Hacer_Read(char *path);
uint32_t Hacer_Release(char *path);
uint32_t Hacer_Write(char *path, char *buffer);
uint32_t Hacer_MKNod(char *path);
uint32_t Hacer_Unlink(char *path);
uint32_t Hacer_MKDir(char *path);
uint32_t Hacer_RMDir(char *path);
uint32_t Hacer_Rename(char *path, char *buffer);

#endif /* SAC_SERVER_H_ */
