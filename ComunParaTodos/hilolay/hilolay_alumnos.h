#ifndef hilolay_alumnos_h__
#define hilolay_alumnos_h__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../../ComunParaTodos/Serializacion/serializacion.h"
#include <commons/collections/queue.h>
#include "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse2/src/SerializacionSUSE/SerializacionSUSE.h"

/**
 * TODO: Interface for alumnos (what they should implement in orde to make this work)
 */
typedef struct hilolay_operations {
	int (*suse_create)(int);
	int (*suse_schedule_next)(void);
	int (*suse_join)(int);
	int (*suse_close)(int);
	int (*suse_wait)(int, char *);
	int (*suse_signal)(int, char *);
} hilolay_operations;

hilolay_operations *main_ops;

void init_internal(struct hilolay_operations*);

t_log* logger;
t_config* archivoConfig;
char* server_ip;
char* server_port;
int socket_suse;

void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores(t_config* archivoConfig);

int suse_create(uint32_t pid);
int suse_schedule_next(uint32_t pid);
int suse_wait(uint32_t pid, char* semID);
int suse_signal(uint32_t pid, char* semID);
int suse_join(uint32_t pid, uint32_t tid);
int suse_close(uint32_t pid, uint32_t tid);

#endif // hilolay_alumnos_h__
