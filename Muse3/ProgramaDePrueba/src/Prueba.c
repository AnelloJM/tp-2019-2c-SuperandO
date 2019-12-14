/*
 ============================================================================
 Name        : Prueba.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libmuse.h>

int main(void) {
	puts("Superand0 Sac-Server Iniciando..."); /* prints Superand0 Sac-Server Iniciando... */
	muse_init(0,"127.0.0.1",5555);
	uint32_t tam = muse_alloc(70);
	printf("tam: %i", tam);
	return EXIT_SUCCESS;
}
