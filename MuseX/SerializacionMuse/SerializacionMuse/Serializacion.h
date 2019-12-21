/*
 * Serializacion.h
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */

#ifndef SERIALIZACIONMUSE_SERIALIZACION_H_
#define SERIALIZACIONMUSE_SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>



//////////////////////////////////////////
//           Tipos de Permisos         //
//////////////////////////////////////////
typedef enum m_operaciones {
	m_INIT,
	m_CLOSE,
	m_ALLOC,
	m_FREE,
	m_GET,
	m_CPY,
	m_MAP,
	m_SYNC,
	m_UNMAP,
	m_RESPONSE,
	m_HANDSHAKE,
} m_operacion;

////////////////////////
// Comunicacion Base //
///////////////////////

typedef struct {
	uint32_t tamanioMensaje;
	m_operacion operaciones;
}__attribute__((packed)) HeaderMuse; //Esta estructura es de tamaño 8

////////////////
// FUNCIONES //
///////////////

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

/**
* ESTA FUNCION ENVIA UN PAQUETE DEL TAMAÑO ESPECIFICADO A TRAVES DEL SOCKET ESPECIFICADO
*/

bool Muse_PackAndSend(int socketCliente, const void*path, uint32_t tamPath, m_operacion operacion);

/**
* ESTA FUNCION ENVIA UN PAQUETE ALLOC A TRAVES DEL SOCKET ESPECIFICADO
*/

void* Muse_PackAndSend_Alloc(int socketCliente, const void *path, uint32_t tamPath, uint32_t tamAlloc, m_operacion operacion);

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

/**
* ESTA FUNCION RETORNA UNA ESTRUCTURA DEL TIPO
* HEADERFUSE DE LA CUAL PODEMOS OBTENER EL TIPO
* DE MENSAJE Y EL TAMANIO DEL MISMO, ESTE ULTIMO
* PARAMETRO DEBEREMOS PASARSELO A LA FUNCION
* Muse_ReceiveAndUnpack PARA QUE NOS DE EL RESTO
* DEL PAQUETE
*/

HeaderMuse Muse_RecieveHeader(int socketCliente);

/**
* ESTA FUNCION RECIBE UN PAQUETE A TRAVES DEL
* SOCKET, TENER EN CUEENTA QUE SEGUN EL TIPO DE
* OPERACION, EL PAQUETE RECIBIDO SERA DISTINTO
* EN ALGUNOS CASOS SERA SOLO UN PATH ASI QUE PODREMOS
* USARLO DIRECTAMENTE, EN OTROS HARA FALTA LLAMAR
* A OTRAS FUNCIONES DE DESEMPAQUETADO
*/

void* Muse_ReceiveAndUnpack(int socketCliente, uint32_t tamanioChar);


#endif /* SERIALIZACIONMUSE_SERIALIZACION_H_ */
