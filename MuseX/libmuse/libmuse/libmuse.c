/*
 * libmuse.c
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */
#include "libmuse.h"
#include <Conexiones/Conexiones.h>
#include <SerializacionMuse/Serializacion.h>
#include <commons/string.h>
#include <commons/log.h>

t_log* logger;
int conexion;
char* id;

void iniciar_logger(){
	logger = log_create("libmuse.log", "libmuse", 1, LOG_LEVEL_INFO);
	log_info(logger, "::::::Se ha creado un nuevo logger::::::");
}

int muse_init(int id_dada, char* ip, int puerto){

	iniciar_logger();
	char* id_dada_en_char = string_itoa(id_dada);
	id = string_duplicate(id_dada_en_char);
	char* puerto_en_char = string_itoa(puerto);
	conexion = conectarse_a_un_servidor(ip,puerto_en_char,logger);
	if(conexion == -1){
		return -1;
	}
	Muse_PackAndSend(conexion,id_dada_en_char,strlen(id_dada_en_char)+1,m_INIT);
	return 0;
}
void muse_close(){
	Muse_PackAndSend(conexion,"close",strlen("close")+1,m_CLOSE);
    close(conexion);
    log_info(logger, ":::Cerrado el socket:::");
    log_info(logger, ":::Cerrando el log:::");
    log_destroy(logger);
}
uint32_t muse_alloc(uint32_t tam){
	Muse_PackAndSend_Alloc(conexion,id,strlen(id)+1,tam,m_ALLOC);
	HeaderMuse headerRecibido;
	//sem_wait(&mutex_buffer);
	headerRecibido = Muse_RecieveHeader(conexion);
	log_info(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
	log_info(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
	uint32_t tamPack = headerRecibido.tamanioMensaje;
	void *packRecibido= Muse_ReceiveAndUnpack(conexion, tamPack);
	//sem_post(&mutex_buffer);
	uint32_t response = Muse_Unpack_Response_Uint32(packRecibido);

	log_info(logger, "Alloc: %i", response);

	free(packRecibido);
	return response;
}
void muse_free(uint32_t dir){

}
int muse_get(void* dst, uint32_t src, size_t n){
    return 0;
}
int muse_cpy(uint32_t dst, void* src, int n){
    return 0;
}
uint32_t muse_map(char *path, size_t length, int flags){
    return 0;
}
int muse_sync(uint32_t addr, size_t len){
    return 0;
}
int muse_unmap(uint32_t dir){
    return 0;
}

