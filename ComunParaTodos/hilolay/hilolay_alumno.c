#include "hilolay_alumnos.h"

//CONEXION CON SUSE

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.log";
	char* nombreArch = "hilolay";
	hilolay_logger = log_create(logPath, nombreArch, 1, LOG_LEVEL_INFO);
	log_info(hilolay_logger, "El logger se creo con exito");
}

void leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		log_error(hilolay_logger,"ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(hilolay_logger,"La configuracion fue cargada exitosamente");
}

void setearValores(t_config* archivoConfig){
	server_ip = config_get_string_value(archivoConfig,"SERVER_IP");
	server_port = config_get_string_value(archivoConfig,"SERVER_PORT");
}

//ENVIO DE PAQUETES

int suse_create(uint32_t pid){
	if(Suse_PackAndSend_Create(socket_suse2, pid)){
		log_info(hilolay_logger,"Se pudo enviar el paquete create suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

int suse_schedule_next(uint32_t pid){
	if(Suse_PackAndSend_Schedule_Next(socket_suse2, pid)){
		log_info(hilolay_logger,"Se pudo enviar el paquete a suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

int suse_wait(uint32_t pid, char* semID){
	if(Suse_PackAndSend_Wait(socket_suse2, pid, semID)){
		log_info(hilolay_logger,"Se pudo enviar el paquete a suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

int suse_signal(uint32_t pid, char* semID){
	if(Suse_PackAndSend_Signal(socket_suse2, pid, semID)){
		log_info(hilolay_logger,"Se pudo enviar el paquete a suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

int suse_join(uint32_t pid, uint32_t tid){
	if(Suse_PackAnd_Send_Join(socket_suse2, pid, tid)){
		log_info(hilolay_logger,"Se pudo enviar el paquete a suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

int suse_close(uint32_t pid, uint32_t tid){
	if(Suse_PackAnd_Send_Close(socket_suse2, pid, tid)){
		log_info(hilolay_logger,"Se pudo enviar el paquete a suse");
		return 0;
	}
	log_error(hilolay_logger, "No se pudo enviar el paquete a suse");
	return -1;
}

static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_schedule_next,
		.suse_join = &suse_join,
		.suse_close = &suse_close,
		.suse_wait = &suse_wait,
		.suse_signal = &suse_signal
};


void hilolay_init(){
	crearLogger();
	leerArchivoDeConfiguracion();
	socket_suse2 = conectarse_a_un_servidor(server_ip, server_port,hilolay_logger);
	printf("====ACA=====\n");
	init_internal(&hiloops);
	printf("====ACA=====\n");
}
