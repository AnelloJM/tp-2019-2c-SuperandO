#ifndef SERIALIZACION_SUSE_SERIALIZACION_SUSE_H_
#define SERIALIZACION_SUSE_SERIALIZACION_SUSE_H_
#define _FILE_OFFSET_BITS 64
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

typedef enum f_operaciones {
	S_INIT,
	S_CREATE, 	//recibe pid
	S_SCHEDULE_NEXT, 	//recibe pid
	S_WAIT, 	//recibe 1°pid y 2°semID
	S_SIGNAL,	 //recibe 1°pid y 2°semID
	S_JOIN, 	//recibe 1°pid y 2°tid
	S_CLOSE, 	//recibe 1°pid y 2°tid
} s_operacion;

typedef struct {
	uint32_t tamanioMensaje;
	s_operacion operaciones;
}__attribute__((packed)) HeaderSuse; //Esta estructura es de tamaño 8


bool Suse_PackAndSend(int socketCliente, const void *paquete, uint32_t tamPaquete, s_operacion operacion); //Empaqueta y manda generico
HeaderSuse Suse_RecieveHeader(int socketCliente); //Desempaqueta el header con el operacion
void* Suse_ReceiveAndUnpack(int socketCliente, uint32_t tamanio); //Desempaqueta el void* paquete

bool Suse_PackAndSend_Create(int socketCliente, uint32_t pid); //Empaqueta un  PID para suse_create y lo envia
bool Suse_PackAndSend_Schedule_Next(int socketCliente, uint32_t pid); //Empaqueta un PID para suse_schedule_next y lo envia
uint32_t Suse_Unpack_Uint32_pid(void *pack); //Desempaqueta un PID de un paquete

bool Suse_PackAndSend_Wait(int socketCLiente, uint32_t pid, char* semaforoID); //Empaqueta un pid y un semID para wait y lo envia
bool Suse_PackAndSend_Signal(int socketCLiente, uint32_t pid, char* semaforoID); //Empaqueta un pid y un semID para signal y lo envia
char* Suse_Unpack_Char(void *pack); //Desempaqueta un char* despues de recibir un PID

bool Suse_PackAnd_Send_Join(int socketCliente, uint32_t pid, uint32_t tid); //Empaqueta un pid y un tid para join y lo envia
bool Suse_PackAnd_Send_Close(int socketCliente, uint32_t pid, uint32_t tid); //Empaqueta un pid y un tid para close y lo envia
uint32_t Suse_Unpack_Uint32_tid(void *pack); //Desempaqueta un TID, despues de recibir un PID


#endif /* SERIALIZACION_FELISYSTEM_SERIALIZACION_FELISYSTEM_H_ */
