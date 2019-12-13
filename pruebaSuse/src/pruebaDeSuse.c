/*
 ============================================================================
 Name        : pruebaDeSuse.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <hilolay/hilolay_alumnos.h>
#include <commons/log.h>
#include <commons/config.h>

int main(void) {
	puts("Probando SUSE..."); /* prints Superand0 Sac-Server Iniciando... */
	hilolay_init();

	log_info(logger, "Me conectare al puerto: %s", server_port);

	suse_create(0);
	suse_schedule_next(0);
	suse_wait(0, strdup("SEMID"));
	suse_signal(0, strdup("SEMID"));
	suse_join(0, 0);
	suse_close(0, 0);
	return EXIT_SUCCESS;
}
