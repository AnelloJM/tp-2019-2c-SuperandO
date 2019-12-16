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
#include <hilolay/hilolay.h>
#include <commons/log.h>
#include <commons/config.h>

int f1(){
	log_info(hilolay_logger,"Hola, soy un suse create exitoso");
	return 0;
}


int main(void) {
	puts("Probando SUSE..."); /* prints Superand0 Sac-Server Iniciando... */
	hilolay_init();
	log_info(hilolay_logger, "Me conectare al puerto: %s", server_port);
	hilolay_t *threadCreate =malloc(sizeof(hilolay_t));
	hilolay_create(threadCreate,NULL,(void*)f1,NULL);
	//hilolay_sem_t* semaforoPrueba;
	//semaforoPrueba = hilolay_sem_open(strdup("A"));
	//hilolay_wait(semaforoPrueba);
	//hilolay_signal(semaforoPrueba);
	hilolay_join(threadCreate);
	//int respuesta = hilolay_return(0);  //En las pruebas esta como return hilolay_return(0);
	return EXIT_SUCCESS;
}
