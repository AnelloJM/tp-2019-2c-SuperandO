/*
 * Serializacion.c
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */


#include "Serializacion.h"

///////////////////////////
// FUNCIONES PARA ENVIAR //
///////////////////////////

bool Muse_PackAndSend(int socketCliente, const void *path, uint32_t tamPath, m_operacion operacion) {

	uint32_t tamMessage = tamPath + sizeof(m_operacion) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &operacion ,sizeof(m_operacion));
	desplazamiento += sizeof(m_operacion);
	memcpy(buffer+desplazamiento, &tamPath , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, path, tamPath);
	desplazamiento += tamPath;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

void* Muse_PackAndSend_Alloc(int socketCliente, const void *path, uint32_t tamPath, uint32_t tamAlloc, m_operacion operacion) {

	uint32_t tamMessage = tamPath + sizeof(m_operacion) + sizeof(uint32_t) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &operacion ,sizeof(m_operacion));
	desplazamiento += sizeof(m_operacion);
	memcpy(buffer+desplazamiento, &tamPath , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, path, tamPath);
	desplazamiento += tamPath;
	memcpy(buffer+desplazamiento, &tamAlloc ,sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}
////////////////////////////
// FUNCIONES PARA RECIBIR //
////////////////////////////

HeaderMuse Muse_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(m_operacion) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(m_operacion) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderMuse headerQueRetorna;
		headerQueRetorna.operaciones = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		free(buffer);
		return headerQueRetorna;
	}
	uint32_t tamanioMensaje = 0;
	m_operacion operacion;
	memcpy(&operacion,buffer,sizeof(m_operacion));
	memcpy(&tamanioMensaje, buffer+(sizeof(m_operacion)), (sizeof(uint32_t)));
	free(buffer);
	HeaderMuse headerQueRetorna;
	headerQueRetorna.operaciones = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

void* Muse_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}



