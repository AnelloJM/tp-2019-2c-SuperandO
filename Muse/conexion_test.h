

#ifndef CONEXION_TEST_H_
#define CONEXION_TEST_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int iniciar_servidor(char* ip, int puerto);//Retorna la conexion del socket servidor

#endif
