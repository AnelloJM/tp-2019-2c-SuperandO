#include <MUSE.h>

void main(){

	char* MUSE_log_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.log");
	crearLogger(MUSE_log_ruta, "MUSE", 0);
	char* MUSE_config_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.cfg");
	leerConfig(MUSE_config_ruta,logger);
	setearValores(MUSE_config_ruta);
	int socket_servidor = iniciar_servidor(listen_ip,listen_port,logger);
	int socket_cliente = esperar_cliente_con_accept(socket_servidor,logger);

void setearValores(t_config * archivoConfig) {
	listen_ip = strdup(config_get_string_value(archivoConfig, "LISTEN_IP"));
	listen_port = config_get_int_value(archivoConfig, "LISTEN_PORT");
	memory_size = config_get_int_value(archivoConfig,"MEMORY_SIZE");
	page_size = config_get_int_value(archivoConfig, "PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfig, "SWAP_SIZE");

	log_info(logger,"Se cargó correctamente el archivo de configuracion");
}
