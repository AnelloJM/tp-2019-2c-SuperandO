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

bool Fuse_PackAndSend(int socketCliente, const void *path, uint32_t tamPath, f_operacion operacion) {

	uint32_t tamMessage = tamPath + sizeof(f_operacion) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
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

bool Fuse_PackAndSend_Write(int socketCliente, const char *buf, size_t size, off_t offset) {
	uint32_t tamMessage = strlen(buf) + 1 + sizeof(size_t) + sizeof(off_t);
	uint32_t tamBuf = (strlen(buf) +1);
	void* buffer = malloc ( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &tamBuf, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer, buf, tamBuf);
	desplazamiento += (strlen(buf)+1);
	memcpy(buffer, &size, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	memcpy(buffer, &offset, sizeof(off_t));
	int resultado = Fuse_PackAndSend(socketCliente, buffer, tamMessage, f_WRITE);
	free(buffer);
	return resultado;
}

////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderFuse Fuse_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(f_operacion) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(f_operacion) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderFuse headerQueRetorna;
		headerQueRetorna.operaciones = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		return headerQueRetorna;
	}
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

char* Fuse_ReceiveAndUnpack_Write_Buf(int socketCliente) {
	uint32_t tamanioBuf;
	recv(socketCliente, &tamanioBuf, sizeof(uint32_t), MSG_WAITALL);
	char *buf = malloc(tamanioBuf);
	recv(socketCliente, buf, tamanioBuf, MSG_WAITALL);
	return buf;
}

size_t Fuse_ReceiveAndUnpack_Write_Size(int socketCliente) {
	size_t size;
	recv(socketCliente, &size, sizeof(size_t), MSG_WAITALL);
	return size;
}

off_t Fuse_ReceiveAndUnpack_Write_offset(int socketCliente) {
	off_t offset;
	recv(socketCliente, &offset, sizeof(off_t), MSG_WAITALL);
	return offset;
}
