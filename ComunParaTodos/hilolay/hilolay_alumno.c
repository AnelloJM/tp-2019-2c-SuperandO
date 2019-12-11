#include "hilolay_alumnos.h"

void hilolay_init(){
	crearLogger();
	leerArchivoDeConfiguracion();
	socket_suse = conectarse_a_un_servidor(server_ip, server_port,logger);
}

void crearLogger(){
	char* logPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.log";
	char* nombreArch = "hilolay";
	logger = log_create(logPath, nombreArch, 1, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
}

void leerArchivoDeConfiguracion(){
	char* configPath = "/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/hilolay/hilolay.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		log_error(logger,"ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
}

void setearValores(t_config* archivoConfig){
	server_ip = config_get_string_value(archivoConfig,"SERVER_IP");
	server_port = config_get_string_value(archivoConfig,"SERVER_PORT");
}

//ENVIO DE PAQUETES

int suse_create(int pid){
	paquete->header.tipoMensaje= "SUSE_CREATE";
	paquete->header.tamanioMensaje = sizeof(pid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &pid, sizeof(pid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}

int suse_schedule_next(void){
	paquete->header.tipoMensaje= "SUSE_SCHEDULE_NEXT";
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}

int suse_wait(int pid, char* sem_name){
	paquete->header.tipoMensaje= "SUSE_WAIT";
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &sem_name, sizeof(sem_name));
	paquete->header.tamanioMensaje = sizeof(sem_name);
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}

int suse_signal(int pid, char* sem_name){
	paquete->header.tipoMensaje= "SUSE_SIGNAL";
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &sem_name, sizeof(sem_name));
	paquete->header.tamanioMensaje = sizeof(sem_name);
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}

int suse_join(int tid){
	paquete->header.tipoMensaje= "SUSE_JOIN";
	paquete->header.tamanioMensaje = sizeof(tid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &tid, sizeof(tid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}

int suse_close(int tid){
	paquete->header.tipoMensaje= "SUSE_CLOSE";
	paquete->header.tamanioMensaje = sizeof(tid);
	void* mensaje = paquete->mensaje;
	memcpy(mensaje, &tid, sizeof(tid));
	int resultado = EnviarPaquete(socket_suse,paquete);
	if (resultado == 0 ){
		log_error(logger,"Falló el envio del paquete");
	}
	log_info(logger,"El paquete se envió correctamente");
	return 0;
}
