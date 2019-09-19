//#include <ComunParaTodos/conexiones.h>
#include <ComunParaTodos/config.h>
#include <ComunParaTodos/log.h>
#include <stdbool.h>

//#ifndef MUSE_MUSE_H_
//#define MUSE_MUSE_H

/*FUNCIONES*/

void main();
void setearValores();

/*VARIABLES*/
char* listen_ip;
int listen_port;
int memory_size;
int page_size;
int swap_size;

int socket_servidor;
int socket_cliente;
t_log * logger;



//#endif /* MUSE_MUSE_H_ */
