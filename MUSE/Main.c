#include <MUSE.h>

void main(){
	char* MUSE_log_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.log");
	crearLogger(MUSE_log_ruta, "MUSE", 0);
	char* MUSE_config_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.cfg");
	leerConfig(MUSE_config_ruta,logger);
	setearValores(MUSE_config_ruta);
	int socket_servidor = iniciar_servidor(listen_ip,listen_port,logger);
	int socket_cliente = esperar_cliente_con_accept(socket_servidor,logger);
}
