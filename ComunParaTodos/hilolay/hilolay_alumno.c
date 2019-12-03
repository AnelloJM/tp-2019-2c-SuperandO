#include "hilolay_alumnos.h"

void hilolay_init(){
	crearLogger();
	leerArchivoDeConfiguracion();
	socket_suse = conectarse_a_un_servidor(server_ip, server_port,logger);
}

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.log";
	char* nombreArch = "hilolay";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}

void leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		log_error(logger,"ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
	config_destroy(archivoConfig);
}

void setearValores(t_config* archivoConfig){
	server_ip = strdup(config_get_string_value(archivoConfig,"SERVER_IP"));
	server_port = strdup(config_get_string_value(archivoConfig,"SERVER_PORT"));
}

//ENVIO DE PAQUETES

int suse_create(int tid){
	paquete->header.tipoMensaje= "SUSE_CREATE";
	paquete->header.tamanioMensaje = sizeof(tid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &tid, sizeof(tid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}

int suse_schedule_next(void){
	paquete->header.tipoMensaje= "SUSE_SCHEDULE_NEXT";
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}

int suse_wait(int tid, char* sem_name){
	paquete->header.tipoMensaje= "SUSE_WAIT";
	paqueteSemaforo* paqueteSem;
	paqueteSem->tid = tid;
	paqueteSem->sem_name = sem_name;
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &paqueteSem, sizeof(paqueteSem));
	paquete->header.tamanioMensaje = sizeof(paqueteSem);
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}

int suse_signal(int tid, char* sem_name){
	paquete->header.tipoMensaje= "SUSE_SIGNAL";
	paqueteSemaforo* paqueteSem;
	paqueteSem->tid = tid;
	paqueteSem->sem_name = sem_name;
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &paqueteSem, sizeof(paqueteSem));
	paquete->header.tamanioMensaje = sizeof(paqueteSem);
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}

int suse_join(int tid){
	paquete->header.tipoMensaje= "SUSE_JOIN";
	paquete->header.tamanioMensaje = sizeof(tid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &tid, sizeof(tid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}

int suse_close(int tid){
	paquete->header.tipoMensaje= "SUSE_CLOSE";
	paquete->header.tamanioMensaje = sizeof(tid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &tid, sizeof(tid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_info(logger,"Falló el envio del paquete");
	}
	return 0;
}
