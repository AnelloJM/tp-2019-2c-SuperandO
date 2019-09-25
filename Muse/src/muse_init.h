#ifndef MUSE_INIT_H
#define MUSE_INIT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h" //esta libreria se define asi porque está en nuestro workspace
//#include "../../ComunParaTodos/Conexiones/Conexiones.c" // esta la sigo poniendo por mi editor, ignorenla
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

t_log* logger;
t_config* archivoConfig;

/*VARIABLES*/
int id;
int puerto;
char* ip;


/* FUNCIONES */
void leerArchivoDeConfiguracion();
void setearValores(t_config* archivoConfig);
int muse_init(int id, char* ip, int puerto, t_log* logger);
void crearLogger();
void loggear_valores(void valor,void archivo_log); //archivo_log puede ser definido por defecto

#endif
