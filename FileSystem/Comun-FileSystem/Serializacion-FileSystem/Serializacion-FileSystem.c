/*
 * Serializacion-FeliSystem.c
 *
 *  Created on: 27 oct. 2019
 *      Author: utnso
 */


#include "Serializacion-FileSystem.h"

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Fuse_PackAndSend_Path(int socketCliente, const char *path, f_operacion operacion) {

	uint32_t tamMessage = strlen(path) + 1 + sizeof(f_operacion) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	uint32_t tamPath = strlen(path) +1 ;
	memcpy(buffer, &operacion ,sizeof(f_operacion));
	desplazamiento += sizeof(f_operacion);
	memcpy(buffer+desplazamiento, &tamPath , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, path, tamPath);
	desplazamiento += tamPath;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderFuse Fuse_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(f_operacion) + sizeof(uint32_t));
	recv(socketCliente, buffer, (sizeof(f_operacion) + sizeof(uint32_t)), MSG_WAITALL);
	uint32_t tamanioMensaje = 0;
	f_operacion operacion;
	memcpy(&operacion,buffer,sizeof(f_operacion));
	memcpy(&tamanioMensaje, buffer+(sizeof(f_operacion)), (sizeof(uint32_t)));
	free(buffer);
	HeaderFuse headerQueRetorna;
	headerQueRetorna.operaciones = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

char* Fuse_ReceiveAndUnpack_Path(int socketCliente, uint32_t tamanioChar) {
	void* retorno = malloc(tamanioChar);
	char* charRetornado;
	recv(socketCliente, retorno, tamanioChar, MSG_WAITALL);
	charRetornado = retorno;
	return charRetornado;
}


