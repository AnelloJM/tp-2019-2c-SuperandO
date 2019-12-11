#include "SerializacionSUSE.h"

bool Suse_PackAndSend(int socketCliente, const void *paquete, uint32_t tamPaquete, s_operacion operacion) {
	uint32_t tamMessage = tamPaquete + sizeof(s_operacion) + sizeof(uint32_t);
	void* buffer = malloc( tamMessage );
	int desplazamiento = 0;
	memcpy(buffer, &operacion ,sizeof(s_operacion));
	desplazamiento += sizeof(s_operacion);
	memcpy(buffer+desplazamiento, &tamPaquete , sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento, paquete, tamPaquete);
	desplazamiento += tamPaquete;
		if(desplazamiento != tamMessage){ return (-1); }
	int resultado = send(socketCliente, buffer, tamMessage, 0);
	free(buffer);
	return resultado;
}

HeaderSuse Suse_RecieveHeader(int socketCliente){
	void* buffer=malloc(sizeof(s_operacion) + sizeof(uint32_t));
	if(recv(socketCliente, buffer, (sizeof(s_operacion) + sizeof(uint32_t)), MSG_WAITALL) == 0){
		HeaderSuse headerQueRetorna;
		headerQueRetorna.operaciones = (-1);
		headerQueRetorna.tamanioMensaje = 0;
		free(buffer);
		return headerQueRetorna;
	}
	uint32_t tamanioMensaje = 0;
	s_operacion operacion;
	memcpy(&operacion,buffer,sizeof(s_operacion));
	memcpy(&tamanioMensaje, buffer+(sizeof(s_operacion)), (sizeof(uint32_t)));
	free(buffer);
	HeaderSuse headerQueRetorna;
	headerQueRetorna.operaciones = operacion;
	headerQueRetorna.tamanioMensaje = tamanioMensaje;
	return headerQueRetorna;
}

//ESTA SOLO DEVUELVE EL PAQUETE, SI EL PAQUETE TIENE ALGO MAS HAY QUE DESEMPAQUETAR CON OTRA FUNCION
void* Suse_ReceiveAndUnpack(int socketCliente, uint32_t tamanio) {
	void* retorno = malloc(tamanio);
	recv(socketCliente, retorno, tamanio, MSG_WAITALL);
	return retorno;
}

bool Suse_PackAndSend_Create(int socketCliente, uint32_t pid){
	void* pack = malloc(sizeof(uint32_t));
	memcpy(pack, &pid, sizeof(uint32_t));
	int resultado = Suse_PackAndSend(socketCliente, pack, sizeof(uint32_t), S_CREATE);
	free(pack);
	return resultado;
}

bool Suse_PackAndSend_Schedule_Next(int socketCliente, uint32_t pid){
	void* pack = malloc(sizeof(uint32_t));
	memcpy(pack, &pid, sizeof(uint32_t));
	int resultado = Suse_PackAndSend(socketCliente, pack, sizeof(uint32_t), S_SCHEDULE_NEXT);
	free(pack);
	return resultado;
}

//Orden: int 1° , char* 2°
bool Suse_PackAndSend_Wait(int socketCLiente, uint32_t pid, char* semaforoID){
	uint32_t tamMensaje = strlen(semaforoID) + 1 + sizeof(pid);
	uint32_t tamSemID = strlen(semaforoID) + 1;
	void* buffer = malloc (tamMensaje);
	int desplazamiento = 0;
	memcpy(buffer, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, semaforoID, tamSemID);
	int resultado = Suse_PackAndSend(socketCLiente, buffer, tamMensaje, S_WAIT);
	free(buffer);
	return resultado;
}

bool Suse_PackAndSend_Signal(int socketCLiente, uint32_t pid, char* semaforoID){
	uint32_t tamMensaje = strlen(semaforoID) + 1 + sizeof(pid);
	uint32_t tamSemID = strlen(semaforoID) + 1;
	void* buffer = malloc (tamMensaje);
	int desplazamiento = 0;
	memcpy(buffer, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, semaforoID, tamSemID);
	int resultado = Suse_PackAndSend(socketCLiente, buffer, tamMensaje, S_SIGNAL);
	free(buffer);
	return resultado;
}
//orden  1°pid    2°tid
bool Suse_PackAnd_Send_Join(int socketCliente, uint32_t pid, uint32_t tid){
	uint32_t tamMensaje = sizeof(pid) + sizeof(tid);
	void* buffer = malloc (tamMensaje);
	int desplazamiento = 0;
	memcpy(buffer, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &tid, sizeof(uint32_t));
	int resultado = Suse_PackAndSend(socketCliente, buffer, tamMensaje, S_JOIN);
	free(buffer);
	return resultado;
}

bool Suse_PackAnd_Send_Close(int socketCliente, uint32_t pid, uint32_t tid){
	uint32_t tamMensaje = sizeof(pid) + sizeof(tid);
	void* buffer = malloc (tamMensaje);
	int desplazamiento = 0;
	memcpy(buffer, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &tid, sizeof(uint32_t));
	int resultado = Suse_PackAndSend(socketCliente, buffer, tamMensaje, S_CLOSE);
	free(buffer);
	return resultado;
}

char* Suse_Unpack_Char(void *pack){
	uint32_t tamanioSemID;
	uint32_t desplazamiento = 0;
	memcpy(&desplazamiento, pack, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	//Preguntar como calcular el tamaño de lo que falta, el tamanioSemID
	memcpy(&tamanioSemID, pack+desplazamiento, sizeof(uint32_t));
	char* semID = malloc(tamanioSemID);
	memcpy(semID, pack+desplazamiento, tamanioSemID);
	return semID;
}

uint32_t Suse_Unpack_Uint32_tid(void *pack){
	uint32_t tid = 0;
	memcpy(&tid, pack+sizeof(uint32_t), sizeof(uint32_t));
	return tid;
}

uint32_t Suse_Unpack_Uint32_pid(void *pack) {
	uint32_t pid = 0;
	memcpy(&pid, pack, sizeof(uint32_t));
	return pid;
}
