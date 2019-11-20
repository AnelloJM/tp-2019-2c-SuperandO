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

bool Fuse_PackAndSend_Write(int socketCliente,const char *path, const char *buf, size_t size, off_t offset) {
	uint32_t tamMessage = strlen(path) + strlen(buf) + 2 + sizeof(size_t) + sizeof(off_t);
	uint32_t tamBuf = (strlen(buf) +1);
	uint32_t tamPath = strlen(path) + 1;
	void* buffer = malloc ( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &tamPath, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, path, tamPath);
	desplazamiento += tamPath;
	memcpy(buffer + desplazamiento, &tamBuf, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, buf, tamBuf);
	desplazamiento += (strlen(buf)+1);
	memcpy(buffer + desplazamiento , &size, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	memcpy(buffer + desplazamiento, &offset, sizeof(off_t));
	int resultado = Fuse_PackAndSend(socketCliente, buffer, tamMessage, f_WRITE);
	free(buffer);
	return resultado;
}

bool Fuse_PackAndSend_MKDir(int socketCliente, const void *path, const char *nombre) {
	uint32_t tamMessage = strlen(path) + strlen(nombre) + (2*sizeof(uint32_t)) + 2;
	uint32_t tamPath = strlen(path) + 1;
	uint32_t tamNombre = strlen(nombre) + 1;
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &tamPath, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, path, tamPath);
	desplazamiento += tamPath;
	memcpy(buffer + desplazamiento, &tamNombre, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, nombre, tamNombre);
	int resultado = Fuse_PackAndSend(socketCliente, buffer, tamMessage, f_MKDIR);
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

void* Fuse_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}

//////////////////////////////////
// FUNCIONES PARA DESEMPAQUETAR //
/////////////////////////////////

char* Fuse_Unpack_Path(void *buffer) {
	uint32_t tamPath = 0;
	memcpy(&tamPath, buffer, sizeof(uint32_t));
	char *path = malloc(tamPath);
	memcpy(path,buffer+sizeof(uint32_t),tamPath);
	return path;
}

char* Fuse_Unpack_Write_Buf(void *buffer) {
	uint32_t tamanioBuf;
	uint32_t path = 0;
	memcpy(&path, buffer, sizeof(uint32_t));
	path += sizeof(uint32_t);
	memcpy(&tamanioBuf, buffer+path, sizeof(uint32_t));
	char *buf = malloc(tamanioBuf);
	memcpy(buf, buffer+path+sizeof(uint32_t), tamanioBuf);
	return buf;
}

size_t Fuse_Unpack_Write_Size(void *buffer) {
	size_t size;
	uint32_t path = 0;
	uint32_t buf = 0;
	memcpy(&path, buffer, sizeof(uint32_t));
	path += sizeof(uint32_t);
	memcpy(&buf, buffer+path, sizeof(uint32_t));
	buf += sizeof(uint32_t);
	memcpy(&size, buffer+path+buf, sizeof(size_t));
	return size;
}

off_t Fuse_Unpack_Write_offset(void *buffer) {
	off_t offset;
	size_t size;
	uint32_t path = 0;
	uint32_t buf = 0;
	memcpy(&path, buffer, sizeof(uint32_t));
	path += sizeof(uint32_t);
	memcpy(&buf, buffer+path, sizeof(uint32_t));
	buf += sizeof(uint32_t);
	memcpy(&offset, buffer+path+buf+(sizeof(size_t)), sizeof(off_t));
	return offset;
}
