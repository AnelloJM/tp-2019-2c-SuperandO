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
typedef enum f_operaciones {
	f_GETATTR,
	f_READDIR,
	f_READ,
	f_OPEN,
	f_RELEASE,
	f_WRITE,
	f_MKNOD,
	f_UNLINK,
	f_MKDIR,
	f_RMDIR,
	f_CHMOD,
	f_UTIME,
	f_RENAME,
	f_TRUNCATE,
	f_SETXATTR,
	f_RESPONSE,
	f_HANDSHAKE
} f_operacion;

////////////////////////
// Comunicacion Base //
///////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	f_operacion operaciones;
}__attribute__((packed)) HeaderFuse; //Esta estructura es de tamaño 8

////////////////
// FUNCIONES //
///////////////

HeaderFuse Fuse_RecieveHeader(int socketCliente);

bool Fuse_PackAndSend(int socketCliente, const void*path, uint32_t tamPath, f_operacion operacion);

char* Fuse_ReceiveAndUnpack_Path(int socketCliente, uint32_t tamanioChar);

bool Fuse_PackAndSend_Write(int socketCliente, const char *buf, size_t size, off_t offset);

char* Fuse_ReceiveAndUnpack_Write_Buf(int socketCliente);

size_t Fuse_ReceiveAndUnpack_Write_Size(int socketCliente);

off_t Fuse_ReceiveAndUnpack_Write_offset(int socketCliente);



#endif /* SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_ */
