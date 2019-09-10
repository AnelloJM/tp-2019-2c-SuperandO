#include "MUSE.h"

int main{
	char* MUSE_log_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.log");
	crearLogger(MUSE_log_ruta, "MUSE", true, 0);
	char* MUSE_config_ruta = strdup("/home/utnso/tp-2019-2c-SuperandO/MUSE/MUSE.cfg");
	leerConfig(MUSE_config_ruta,logger);
	levantarServidor();
}
