/*
 * Serializacion-FeliSystem.h
 *
 *  Created on: 27 oct. 2019
 *      Author: utnso
 */

#ifndef SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_
#define SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stddef.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>



//////////////////////////////////////////
//           Tipos de Permisos         //
//////////////////////////////////////////
typedef enum f_permisos {
	f_READ,
	f_WRITE,
	f_READWRITE,
	f_HANDSHAKE
} f_permisos;

//////////////////////////////////////////
//           Comunicacion Base          //
//////////////////////////////////////////

typedef struct {
	int tamanioMensaje;
	f_permisos permisos;
}__attribute__((packed)) HeaderFuse;

typedef struct {
	HeaderFuse headerFuse;
	void* mensaje;
}__attribute__((packed)) PaqueteFuse;

//////////////////////////////////////////
//          Estructuras Utiles          //
//////////////////////////////////////////

typedef struct {
	char *path;
	struct stat *stbuf;
}__attribute__((packed)) f_getattr;

typedef struct {
	const char *path;
	void *buf;
	fuse_fill_dir_t filler;
	off_t offset;
	struct fuse_file_info *fi;
}__attribute__((packed)) f_readdir;

typedef struct {
	const char *path;
	struct fuse_file_info *fi;
}__attribute__((packed)) f_open;

typedef struct {
	const char *path;
	char *buf;
	size_t size;
	off_t offset;
	struct fuse_file_info *fi;
}__attribute__((packed)) f_read;

//////////////////////////////////////////
//              Funciones               //
//////////////////////////////////////////

bool EnviarHandshake(int socketFD);

bool EnviarDatosTipo(int socketFD, void* datos, int tamDatos, f_permisos permisos);

bool EnviarPaquete(int socketCliente, PaqueteFuse* paquete);

int RecibirDatos(void* paquete, int socketFD, uint32_t cantARecibir);

int RecibirPaqueteServidorFuse(int socketFD, PaqueteFuse* paquete); //Responde al recibir un Handshake

int RecibirPaqueteCliente(int socketFD, PaqueteFuse* paquete); //No responde los Handshakes

#endif /* SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_ */
