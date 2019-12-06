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
#include <commons/config.h>
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
	ptrGBloque bloques_de_datos [1024];
} Bloque_de_puntero;

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
	char estado;//0: Borrado, 1: Archivo, 2: Directorio
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
/*
* Esta funcion inicializa todas
* las estructuras necesarias y  
* se ocupa de formatear el disco
*/
void iniciar_Sac_Server();
/*
* Esta funcion se ocupa de asignar
* los valores iniciales al header
*/
void iniciar_header();
/*
* Esta funcion se ocupa de 
* colocar la cantidad pasada por 
* parametros de bits correspondientes
* al bitmap en 1
*/
void cargar_bitmap(int cantidad);
/*
* Esta funcion se ocupa de 
* setear el estado de todos los
* nodos en 0 y poner su nombre \0
*/
void iniciar_tabla_de_nodos();

///////////////////////////////
// FUNCIONES ADMINISTRATIVAS //
//////////////////////////////

/*
* Esta funcion se ocupa de
* retornar la hora actual 
*/
uint64_t timestamp();
/*
* Esta funcion es la principal de nuestro servidor
* se ocupa de administrar los pedidos y direccionarlos
* correspondientemente, y responder
*/
void* funcionMagica(int cliente);
/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro
*/
uint32_t tamanio_archivo(char *archivo);
/*
* Esta funcion retorna el tamanio de
* el archivo pasado por parametro en bloques
*/
int tamanio_archivo_en_bloques(uint32_t tamanio);
/*
* Esta funcion es la encargada de
* formatear el disco
*/
void limbiar_bloques_de_datos();

//////////////////////////
// FUNCIONES DEL BITMAP //
/////////////////////////

/*
* Esta funcion retorna el primer
* espacio vacio que encuentre en el bitmap
*/
int buscar_espacio_en_bitmap();
/*
* Esta funcion se encarga de ocupar el bit
* indicado por el indice dado en el bitmap
*/
void ocupar_bloque_en_bitmap(int indice);
/*
* Esta funcion se encarga de liberar el bit
* indicado por el indice dado en el bitmap
*/
void liberar_bloque_en_bitmap(int indice);

////////////////////////////////////
// FUNCIONES DE LA TABLA DE NODOS //
///////////////////////////////////

/*
* Esta funcion contesta si dado el nombre
* pasado por parametros, existe al menos UN
* nodo en la tabla de nodos con ese nombre
*/
bool existe_nodo_con_nombre(char* nombre);

/*
* Esta funcion retorna el nodo correspondiente
* al path pasado por parametro
*/
uint32_t exite_path_retornando_nodo(char* path);
char* obtener_nombre_nodo(uint32_t numero_de_nodo);
/*
* 
* comienza en 1 ya que toma que 0 es el
* directorio raiz
*/
int hallar_nodo_libre();
t_list* hallar_padres(char* nombre_buscado);
ptrGBloque obtener_nodo_padre(uint32_t numero_de_nodo);
char* obtener_nombre_nodo(uint32_t numero_de_nodo);
void crear_directorio_en_padre(uint32_t numero_de_nodo_padre, char *nombre_de_archivo);
uint32_t hallar_nodo_con_nombre_y_padre(char* nombre, uint32_t padre);
void mostrar_hijos_de(char* path);
t_list *hallar_hijos_de_path(char* path);
t_list *hijos_de_nodo(uint32_t nodo_padre);

///////////////////////
// FUNCIONES DE FUSE //
//////////////////////

uint32_t Hacer_Getattr(char *path);
uint32_t Hacer_Getattr_Size(char *path);
char *Hacer_ReadDir(char *path);
uint32_t Hacer_Open(char *path);
char *Hacer_Read(char *path, size_t size, off_t offset);
uint32_t Hacer_Release(char *path);
uint32_t Hacer_Write(char *path, char *buffer, uint32_t ya_escrito_del_buffer);
uint32_t Hacer_MKNod(char *path);
uint32_t Hacer_Unlink(char *path);
uint32_t Hacer_MKDir(char *path);
uint32_t Hacer_RMDir(char *path);
uint32_t Hacer_Rename(char *path, char *buffer);
uint32_t Hacer_Truncate(char *path, uint32_t nuevo_tamanio);

#endif /* SAC_SERVER_H_ */
