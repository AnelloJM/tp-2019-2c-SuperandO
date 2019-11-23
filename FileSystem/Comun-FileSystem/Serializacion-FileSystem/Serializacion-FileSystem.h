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

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Fuse_PackAndSend(int socketCliente, const void*path, uint32_t tamPath, f_operacion operacion);

bool Fuse_PackAndSend_Write(int socketCliente,const char *path, const char *buf, size_t size, off_t offset);

bool Fuse_PackAndSend_MKDir(int socketCliente, const void *path, const char *nombre);

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

/**
* ESTA FUNCION RETORNA UNA ESTRUCTURA DEL TIPO
* HEADERFUSE DE LA CUAL PODEMOS OBTENER EL TIPO
* DE MENSAJE Y EL TAMANIO DEL MISMO, ESTE ULTIMO
* PARAMETRO DEBEREMOS PASARSELO A LA FUNCION
* Fuse_ReceiveAndUnpack PARA QUE NOS DE EL RESTO
* DEL PAQUETE
*/

HeaderFuse Fuse_RecieveHeader(int socketCliente);

/**
* ESTA FUNCION RECIBE UN PAQUETE A TRAVES DEL
* SOCKET, TENER EN CUEENTA QUE SEGUN EL TIPO DE
* OPERACION, EL PAQUETE RECIBIDO SERA DISTINTO
* EN ALGUNOS CASOS SERA SOLO UN PATH ASI QUE PODREMOS
* USARLO DIRECTAMENTE, EN OTROS HARA FALTA LLAMAR
* A OTRAS FUNCIONES DE DESEMPAQUETADO
*/

void* Fuse_ReceiveAndUnpack(int socketCliente, uint32_t tamanioChar);

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////


/**
* ESTA FUNCION SE USA SOLO EN CASO DE QUERER
* RECIBIR UN PATH DE UN PQUETE QUE CONTENGA ALGO
* MAS ADEMAS DEL PATH
*/

char* Fuse_Unpack_Path(void *buffer);

/**
* ESTA FUNCION RETORNA EL BUFFER EN CASO
* DE QUE SE HAYA RECIBIDO UN PAQUETE DEL
* TIPO f_WRITE
*/

char* Fuse_Unpack_Write_Buf(void *buffer);

/**
* ESTA FUNCION RETORNA EL SIZE EN CASO
* DE QUE SE HAYA RECIBIDO UN PAQUETE DEL
* TIPO f_WRITE
*/

size_t Fuse_Unpack_Write_Size(void *buffer);

/**
* ESTA FUNCION RETORNA EL OFFSET EN CASO
* DE QUE SE HAYA RECIBIDO UN PAQUETE DEL
* TIPO f_WRITE
*/

off_t Fuse_Unpack_Write_offset(void *buffer);

#endif /* SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_ */
