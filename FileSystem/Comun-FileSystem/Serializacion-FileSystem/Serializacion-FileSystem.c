/*
 * Serializacion-FeliSystem.c
 *
 *  Created on: 27 oct. 2019
 *      Author: utnso
 */


#include "Serializacion-FileSystem.h"


bool EnviarPaquete(int socketCliente, Paquete* paquete) {
	int cantAEnviar = sizeof(HeaderFuse) + paquete->headerFuse.tamanioMensaje;
	void* datos = malloc(cantAEnviar);

	memcpy(datos, &(paquete->headerFuse), sizeof(HeaderFuse));
	if (paquete->headerFuse.tamanioMensaje > 0){ //No sea t_HANDSHAKE
		memcpy(datos + sizeof(HeaderFuse), (paquete->mensaje), paquete->headerFuse.tamanioMensaje);
	}/*
	int enviado = 0; //bytes enviados
	int totalEnviado = 0;
	bool valor_retorno=true;
	do {
		puts("Por enviar");
		enviado = send(socketCliente, datos + totalEnviado, cantAEnviar - totalEnviado, 0);
		puts("Enviado perro");
		totalEnviado += enviado;
		if(enviado==-1){
			valor_retorno=false;
			break;
		}
	} while (totalEnviado != cantAEnviar);*/
	int enviado = 0;
	bool valor_retorno=true;
	enviado = send(socketCliente, datos,cantAEnviar, 0);
	if(enviado==-1){
		valor_retorno=false;
	}
	free(datos);
	return valor_retorno;
}

bool EnviarDatosTipo(int socketFD, void* datos, int tamDatos, f_permisos permisos){
	Paquete* paquete = malloc(sizeof(Paquete));
	paquete->headerFuse.permisos = permisos;
	uint32_t r = 0;
	bool valor_retorno;
	if(tamDatos<=0 || datos==NULL){
		paquete->headerFuse.tamanioMensaje = sizeof(uint32_t);
		paquete->mensaje = &r;
	} else {
		paquete->headerFuse.tamanioMensaje = tamDatos;
		paquete->mensaje=datos;
	}
	valor_retorno=EnviarPaquete(socketFD, paquete);
	free(paquete);
	return valor_retorno;
}

bool EnviarHandshake(int socketFD) {
	Paquete* paquete = malloc(sizeof(HeaderFuse));
	HeaderFuse header;
	header.permisos = f_HANDSHAKE;
	header.tamanioMensaje = 0;
	paquete->headerFuse = header;
	bool resultado = EnviarPaquete(socketFD, paquete);
	free(paquete);
	return resultado;
}

int RecibirDatos(void* paquete, int socketFD, uint32_t cantARecibir) {
	void* datos = malloc(cantARecibir);
	int recibido = 0;
	int totalRecibido = 0;

	do {
		recibido = recv(socketFD, datos + totalRecibido, cantARecibir - totalRecibido, 0);
		totalRecibido += recibido;
	} while (totalRecibido != cantARecibir && recibido > 0);
	memcpy(paquete, datos, cantARecibir);
	free(datos);
	if (recibido < 0) {
		printf("Cliente Desconectado\n");
		close(socketFD); // ¡Hasta luego!
	} else if (recibido == 0) {
		printf("Fin de Conexion en socket %d\n", socketFD);
		close(socketFD); // ¡Hasta luego!
	}
	return recibido;
}

int RecibirPaqueteServidorFuse(int socketFD, Paquete* paquete) {
	int resul = RecibirDatos(&(paquete->headerFuse), socketFD, sizeof(HeaderFuse));
	if (resul > 0) { //si no hubo error
		if (paquete->headerFuse.permisos == f_HANDSHAKE) { //vemos si es un f_HANDSHAKE
			EnviarHandshake(socketFD);
		} else if (paquete->headerFuse.tamanioMensaje > 0){ //recibimos un payload y lo procesamos (por ej, puede mostrarlo)
			paquete->mensaje = malloc(paquete->headerFuse.tamanioMensaje);
			resul = RecibirDatos(paquete->mensaje, socketFD, paquete->headerFuse.tamanioMensaje);
		}
	}
	return resul;
}

int RecibirPaqueteCliente(int socketFD, Paquete* paquete) {
	paquete->mensaje = NULL;
	int resul = RecibirDatos(&(paquete->headerFuse), socketFD, sizeof(HeaderFuse));
	if (resul > 0 && paquete->headerFuse.permisos != f_HANDSHAKE && paquete->headerFuse.tamanioMensaje > 0) { //si no hubo error ni es un t_HANDSHAKE
		paquete->mensaje = malloc(paquete->headerFuse.tamanioMensaje);
		resul = RecibirDatos(paquete->mensaje, socketFD, paquete->headerFuse.tamanioMensaje);
	}
	return resul;
}

