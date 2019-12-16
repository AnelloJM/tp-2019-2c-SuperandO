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

void iniciar_logger(){
	logger = log_create("libmuse.log", "libmuse", 1, LOG_LEVEL_INFO);
	log_info(logger, "::::::Se ha creado un nuevo logger::::::");
}

void iniciar_configuracion(){

}

char* obtener_id_proceso(int id, char* ip){
	char* id_en_char = string_itoa(id);
	char* id_del_proceso = string_duplicate(ip);
	string_append_with_format(&id_del_proceso,"-%s",id_en_char);
	return id_del_proceso;
}

int muse_init(int id, char* ip, int puerto){

	iniciar_logger();
	iniciar_configuracion();

	char* id_del_proceso = obtener_id_proceso(id, ip);
	log_info(logger,"id_del_proceso: %s", id_del_proceso);
	char* puerto_en_char = string_itoa(puerto);
	int conexion = conectarse_a_un_servidor(ip,puerto_en_char,logger);
	if(conexion == -1){
		return -1;
	}
	return 0;
}
void muse_close(){
    
}
uint32_t muse_alloc(uint32_t tam){
    return 0;
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

