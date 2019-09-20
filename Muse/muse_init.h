#ifndef MUSE_INIT_H
#define MUSE_INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include "conexion_test.h"
#include "conexion_test.c" //esto es medio ilegal, pero si no lo agrego no me reconoce las funciones
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

t_log* g_logger;
t_config* g_config;

int muse_init(int id, char* ip, int puerto);

int leer_id(void *archivo);
char *leer_ip(void *archivo);
int leer_puerto(void *archivo);

void loggear_valores(void valor,void archivo_log); //archivo_log puede ser definido por defecto

#endif
