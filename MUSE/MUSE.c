#include <MUSE.h>

void setearValores(t_config * archivoConfig) {
	listen_ip = strdup(config_get_string_value(archivoConfig, "LISTEN_IP"));
	listen_port = config_get_int_value(archivoConfig, "LISTEN_PORT");
	memory_size = config_get_int_value(archivoConfig,"MEMORY_SIZE");
	page_size = config_get_int_value(archivoConfig, "PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfig, "SWAP_SIZE");

	log_info(logger,"Se cargó correctamente el archivo de configuracion");
}
