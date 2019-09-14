#include "MUSE.h"

void crearLogger(char* file, char *program_name, bool is_active_console, t_log_level level){
	logger = log_create(file,program_name,is_active_console,level);
	free(file);
}

void leerConfig(char* configPath, t_log* logger){
	leerArchivoConfiguracion(configPath,logger);
	log_info(logger, "Se leyo exitosamente el archivo de configuracion");
}

void leerArchivoConfiguracion(char* configPath, t_log* logger){
	t_config* archivoConfiguracion;

	archivoConfiguracion = config_create(configPath);

	if(archivoConfiguracion == NULL){
		perror("No se encontro el archivo de configuracion");
		log_error(logger,"No se encontro el archivo de configuracion");
	}

	setearValores(archivoConfiguracion);
	config_destroy(archivoConfiguracion);
}

void setearValores(t_config* archivoConfiguracion){
	listen_IP = strdup(config_get_string_value(archivoConfiguracion, "LISTEN_IP"));
	listen_port = config_get_int_value(archivoConfiguracion, "LISTEN_PORT");
	memory_size = config_get_int_value(archivoConfiguracion, "MEMORY_SIZE");
	page_size = config_get_int_value(archivoConfiguracion, "PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfiguracion, "SWAP_SIZE");
}
