/*
 ============================================================================
 Name        : Muse.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Muse.h"

void iniciar_logger(){
	logger = log_create("Muse.log", "Muse", 1, LOG_LEVEL_INFO);
	log_info(logger, "::::::Se ha creado un nuevo logger::::::");
}

int main(void) {
	iniciar_logger();
	int servidor, cliente;
	servidor = iniciar_servidor("127.0.0.1","6969",logger);

	cliente = esperar_cliente_con_accept(servidor, logger);

}
