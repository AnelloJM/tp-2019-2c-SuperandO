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
	f_GETATTR,
	f_READDIR,
	f_READ,
	f_OPEN,
	f_HANDSHAKE
} f_operacion;

//////////////////////////////////////////
//           Comunicacion Base          //
//////////////////////////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	f_operacion operaciones;
}__attribute__((packed)) HeaderFuse; //Esta estructura es de tamaño 8

typedef struct {
	HeaderFuse headerFuse;
	void* mensaje;
} PaqueteFuse;

//////////////////////////////////////////
//          Estructuras Utiles          //
//////////////////////////////////////////

typedef struct {
	uint32_t tamPath;
	char *path;
	uint32_t tamStbuf;
	struct stat *stbuf;
} f_getattr;

typedef struct {
	uint32_t tamPath;
	const char *path;
	uint32_t tamBuf;
	void *buf;
	fuse_fill_dir_t filler;
	off_t offset;
	uint32_t tamFi;
	struct fuse_file_info *fi;
} f_readdir;

typedef struct {
	uint32_t tamPath;
	const char *path;
	uint32_t tamFi;
	struct fuse_file_info *fi;
} f_open;

typedef struct {
	uint32_t tamPath;
	const char *path;
	uint32_t tamBuf;
	char *buf;
	size_t size;
	off_t offset;
	uint32_t tamFi;
	struct fuse_file_info *fi;
} f_read;

//////////////////////////////////////////
//              Funciones               //
//////////////////////////////////////////

bool FuseEnviarHandshake(int socketFD);

bool FuseEnviarDatosTipo(int socketFD, void* datos, int tamDatos, f_operacion operaciones);

bool FuseEnviarPaquete(int socketCliente, PaqueteFuse* paquete);

int FuseRecibirDatos(void* paquete, int socketFD, uint32_t cantARecibir);

int FuseRecibirPaqueteServidor(int socketFD, PaqueteFuse* paquete); //Responde al recibir un Handshake

int FuseRecibirPaqueteCliente(int socketFD, PaqueteFuse* paquete); //No responde los Handshakes}

// ------------------------------------------------------------------------------------------------------

HeaderFuse FuseRecibirHeader(int socketCliente);

void FuseEmpaquetarPackGetAttr(const char *path, struct stat *stbuf, PaqueteFuse *pack);

char* FuseDesempaquetarPackGetAttr(int socketCliente, uint32_t tamanio);

#endif /* SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_ */
