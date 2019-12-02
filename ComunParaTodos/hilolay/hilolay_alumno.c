#include "hilolay_alumnos.h"

void hilolay_init(){
	crearLogger();
	leerArchivoDeConfiguracion();
	conectarse_a_un_servidor(server_ip, server_port,logger);
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


/*int suse_create(int tid){
	return 0;
}

int suse_schedule_next(void){
	return 0;
}

int suse_wait(int tid, char* sem_name){
	return 0;
}

int suse_signal(int tid, char* sem_name){
	return 0;
}

int suse_join(int tid){
	return 0;
}

int suse_close(int tid){
	return 0;
}*/
